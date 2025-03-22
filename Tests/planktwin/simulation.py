import numpy as np
import cv2
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Define the 3D point (particle)
particle_3D = np.array([1.0, 0.5, 5.0])  # (X, Y, Z) in world coordinates

# Camera intrinsic parameters (assumed same for both cameras)
focal_length = 800  # Focal length in pixels
image_size = (800, 600)  # Width, Height
principal_point = (image_size[0] / 2, image_size[1] / 2)
K = np.array([[focal_length, 0, principal_point[0]],
              [0, focal_length, principal_point[1]],
              [0, 0, 1]])

# Define camera extrinsic parameters (position and orientation)
camera1_position = np.array([0, -1, 0])  # Camera 1 is at (0, -1, 0)
camera2_position = np.array([0, 1, 0])   # Camera 2 is at (0, 1, 0)

# Camera rotation matrices (both cameras are looking towards the particle but at an angle)
def get_rotation_matrix(angle_degrees):
    angle_radians = np.radians(angle_degrees)
    R = np.array([[np.cos(angle_radians), 0, np.sin(angle_radians)],
                  [0, 1, 0],
                  [-np.sin(angle_radians), 0, np.cos(angle_radians)]])
    return R

R1 = get_rotation_matrix(-15)  # Camera 1 rotated -15 degrees
R2 = get_rotation_matrix(15)   # Camera 2 rotated 15 degrees

# Camera extrinsics (transformation matrices)
T1 = -R1 @ camera1_position.reshape(3, 1)
T2 = -R2 @ camera2_position.reshape(3, 1)

# Projection matrices
P1 = K @ np.hstack((R1, T1))
P2 = K @ np.hstack((R2, T2))

# Convert 3D point to homogeneous coordinates
particle_3D_hom = np.append(particle_3D, 1).reshape(4, 1)

# Project the 3D point onto each camera
particle_2D_cam1 = P1 @ particle_3D_hom
particle_2D_cam2 = P2 @ particle_3D_hom

# Convert to pixel coordinates
particle_2D_cam1 /= particle_2D_cam1[2]
particle_2D_cam2 /= particle_2D_cam2[2]

# Display results
print("Projected 2D coordinates in Camera 1:", particle_2D_cam1[:2].flatten())
print("Projected 2D coordinates in Camera 2:", particle_2D_cam2[:2].flatten())

fig = plt.figure(figsize=(8, 8))

# plt.show()

# 3D Visualization
ax = fig.add_subplot(211, projection='3d')

# Plot particle
ax.scatter(*particle_3D, color='red', s=100, label='Particle')

# Plot cameras
ax.scatter(*camera1_position, color='green', s=100, label='Camera 1')
ax.scatter(*camera2_position, color='blue', s=100, label='Camera 2')

# Draw lines from cameras to particle
ax.plot([camera1_position[0], particle_3D[0]],
        [camera1_position[1], particle_3D[1]],
        [camera1_position[2], particle_3D[2]], 'g--')
ax.plot([camera2_position[0], particle_3D[0]],
        [camera2_position[1], particle_3D[1]],
        [camera2_position[2], particle_3D[2]], 'b--')

# Labels and visualization settings
ax.set_xlabel("X Axis")
ax.set_ylabel("Y Axis")
ax.set_zlabel("Z Axis")
ax.set_title("3D Stereo Camera Setup")
ax.legend()



# Plot the projections
# fig, axs = plt.subplots(1, 2, figsize=(10, 5))
ax1 = fig.add_subplot(223)
ax1.scatter(*particle_2D_cam1[:2], color='red', label='Particle')
ax1.set_title("Camera 1 View")
ax1.set_xlim([0, image_size[0]])
ax1.set_ylim([image_size[1], 0])
ax1.legend()

ax2 = fig.add_subplot(224)
ax2.scatter(*particle_2D_cam2[:2], color='blue', label='Particle')
ax2.set_title("Camera 2 View")
ax2.set_xlim([0, image_size[0]])
ax2.set_ylim([image_size[1], 0])
ax2.legend()



plt.show()

