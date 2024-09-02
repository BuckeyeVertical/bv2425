# Buckeye Vertical 2024-2025 Codebase

What is included in the Docker image is
* Ubuntu 22.04 (jammy)
* ROS 2 Humble
* Gazebo Garden
* Development tools required by PX4 firmware

# Prerequisites
Windows (GPU):
1) Install WSL
2) Install docker within wsl
3) Install nvidia CUDA
4) Install nvidia container toolkit

Windows:
1) Install WSL
2) Install docker within wsl

Macos:
1) Install docker

# Build Docker Image
Docker should be installed before proceeding with the next steps
You can follow [this link](https://docs.docker.com/engine/install/ubuntu/) for docker setup on Ubuntu

* Clone this package `git clone https://github.com/BuckeyeVertical/bv2425.git`
* Build the docker image
    ```bash 
    cd bv2425/docker
    make bv-dev
    ```

This builds a Docker image that has the required PX4 development environment, and ROS 2 Humble Desktop. It does not container the PX4 source code or any ROS 2 workspaces. This is covered in the following sections.

The Gazebo version in the provided Docker image is Gazebo Garden.

# Run
If you have NVIDIA GPU, run
```bash
./docker_run_nvidia.sh
```
Otherwise, run
```bash
./docker_run.sh
```

**NOTE**

* Source files and workspaces should be saved inside a shared volume. The path to the shared volume inside the container is `/home/user/shared_volume`. The path to the shared volume in the host is `$HOME/bvdock_shared_volume`.
* When you login inside the container, the username is `user` and the passwrod is `user`. `user` is part of the `sudo` group and can install pckages using `sudo apt install`

# Install PX4
It's recommended to have the `PX4-Autopilot` src and the ros 2 workspace(s) inside the shared volume, so you don't lose them if the container is removed.

* Enter the container `./docker_run_nvidia.sh`
* Go to the shared volume `cd /home/user/shared_volume`
* Clone the `PX4-Autopilot` source 
    ```bash
    git clone https://github.com/PX4/PX4-Autopilot.git --recursive
    ```
* Enter the `PX4-Autopilot` directory, clean and build the source code
    ```bash
    cd PX4-Autopilot
    make clean
    make distclean
    make submodulesclean
    ```
* Test if PX4 can run  with the Gazebo simiulation 
    ```bash
    make px4_sitl gz_x500
    ```
    The XRCE-DDS client already runs with the above command
* The XRCE-DDS Agent is already built with the Docker image. In a new terminal, run the XRCE-DDS Agent
    ```bash
    MicroXRCEAgent udp4 -p 8888
    ```
    The agent and client are now running and they should connect.

# Build & Run ROS 2 Wrokspace
* You can build/run ROS 2 workspace as described [here](https://docs.px4.io/main/en/ros/ros2_comm.html#build-ros-2-workspace)

# Developing with dev container
* Install docker & devcontainer plugins in vscode
* Make sure the docker is running (you can check by running `docker ps`)
* Use `Ctrl + shift + P` and type `Attach to Running Container...` and click enter
* Pick the container name when prompted
* Once the new window pops up, click `Open Folder`
* Open the shared volume

# Examples
* [Offboard control with PX4](https://github.com/Jaeyoung-Lim/px4-offboard)
    * Use `MicroXRCEAgent udp4 -p 8888` instead of `micro-ros-agent udp4 --port 8888`
    * Use `make px4_sitl gz_x500` instead of `make px4_sitl gazebo`
    * You will need to download and run QGroundControl (does not have to be inside the container). And then arm the vehicle and switch the flight mode to Offboard

# Gazebo Simulation
## Useful Links
* [Gazebo library of simulations worlds and models](https://app.gazebosim.org/fuel)

## Acknowledgements

This project uses components from [px4_ros2_humble](https://github.com/mzahana/px4_ros2_humble) by [mzahana]. The original work is licensed under [MIT License].