# Adapted from https://github.com/mzahana/px4_ros2_humble
# Original code by mzahana

.PHONY: bv-dev bv-prod

all: bv-dev bv-prod

bv-dev:
	docker build -t bv-dev . -f Dockerfile_simulation-ubuntu22

bv-prod:
	docker build -t bv-prod . -f Dockerfile_ros2-humble