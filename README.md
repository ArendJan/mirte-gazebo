```bash
echo "deb [trusted=yes] https://github.com/ArendJan/mirte-gazebo/raw/ros_mirte_humble_jammy_amd64/ ./" | sudo tee /etc/apt/sources.list.d/ArendJan_mirte-gazebo.list
echo "yaml https://github.com/ArendJan/mirte-gazebo/raw/ros_mirte_humble_jammy_amd64/local.yaml humble" | sudo tee /etc/ros/rosdep/sources.list.d/1-ArendJan_mirte-gazebo.list
```
