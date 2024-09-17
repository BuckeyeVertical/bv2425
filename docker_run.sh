#! /bin/bash
# Runs a docker container for PX4 + ROS 2 Humble dev environment
# Requires:
#   - docker
#   - nvidia-docker
#   - an X server
# Optional:
#   - device mounting such as: joystick mounted to /dev/input/js0
#
# Adapted from https://github.com/mzahana/px4_ros2_humble
# Original code by mzahana

# DOCKER_REPO="mzahana/px4-dev-simulation-ubuntu22"
DOCKER_REPO="bv-dev"
CONTAINER_NAME="bvdock"
WORKSPACE_DIR=~/${CONTAINER_NAME}_shared_volume
DOCKER_OPTS=""


# This will enable running containers with different names
# It will create a local workspace and link it to the image's catkin_ws
if [ "$1" != "" ]; then
    CONTAINER_NAME=$1
fi
WORKSPACE_DIR=~/${CONTAINER_NAME}_shared_volume
if [ ! -d $WORKSPACE_DIR ]; then
    mkdir -p $WORKSPACE_DIR
fi
echo "Container name:$CONTAINER_NAME WORSPACE DIR:$WORKSPACE_DIR" 


if [ "$2" != "" ]; then
    CMD=$2
fi

XAUTH=/tmp/.docker.xauth
xauth_list=$(xauth nlist :0 | sed -e 's/^..../ffff/')
if [ ! -f $XAUTH ]
then
    echo XAUTH file does not exist. Creating one...
    touch $XAUTH
    chmod a+r $XAUTH
    if [ ! -z "$xauth_list" ]
    then
        echo $xauth_list | xauth -f $XAUTH nmerge -
    fi
fi

# Prevent executing "docker run" when xauth failed.
if [ ! -f $XAUTH ]
then
  echo "[$XAUTH] was not properly created. Exiting..."
  exit 1
fi


echo "Shared WORKSPACE_DIR: $WORKSPACE_DIR";
 
echo "Starting Container: ${CONTAINER_NAME} with REPO: $DOCKER_REPO"

xhost +local:root

if [ "$2" != "" ]; then
    CMD=$2
fi
echo $CMD
if [ "$(docker ps -aq -f name=${CONTAINER_NAME})" ]; then
    if [ "$(docker ps -aq -f status=exited -f name=${CONTAINER_NAME})" ]; then
        # cleanup
        echo "Restarting the container..."
        docker start ${CONTAINER_NAME}
    fi

    docker exec --user user -it ${CONTAINER_NAME} bash -c "/bin/bash ${CMD}"

else
    CMD="/bin/bash"

    echo "Running container ${CONTAINER_NAME}..."
    #-v /dev/video0:/dev/video0 \
    docker run -it \
        --network host \
        -e LOCAL_USER_ID="$(id -u)" \
        --env="DISPLAY=$DISPLAY" \
        --env="QT_X11_NO_MITSHM=1" \
        -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
        -v /dev/dri:/dev/dri\
        --volume="$WORKSPACE_DIR:/home/user/shared_volume:rw" \
        --volume="/dev/input:/dev/input" \
        --volume="$XAUTH:$XAUTH" \
        -env="XAUTHORITY=$XAUTH" \
        --publish 14556:14556/udp \
        --name=${CONTAINER_NAME} \
        --privileged \
        ${DOCKER_REPO} \
        bash -c "${CMD}"
fi

