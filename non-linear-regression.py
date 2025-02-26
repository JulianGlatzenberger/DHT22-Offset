import numpy as np
import matplotlib.pyplot as plt

# Given data
T = np.array([7.9,19.1, 22.1])  # Temperatures
O = np.array([1.9,2.7, 1.1])  # Offsets

# Fit a quadratic polynomial (2nd degree)
coeffs = np.polyfit(T, O, 2)  # Finds coefficients for O = aT^2 + bT + c
poly_func = np.poly1d(coeffs)  # Create polynomial function

# Generate fitted values for visualization
T_fit = np.linspace(min(T), max(T), 100)  # Smooth curve
O_fit = poly_func(T_fit)  # Calculate offsets using the quadratic equation

# Plot
plt.scatter(T, O, color='red', label="Data Points")  # Original data points
plt.plot(T_fit, O_fit, label=f"Quadratic Fit: {coeffs[0]:.4f}T² + {coeffs[1]:.4f}T + {coeffs[2]:.4f}", color='blue')
plt.xlabel("Temperature (°C)")
plt.ylabel("Offset")
plt.legend()
plt.grid(True)
plt.title("Non-Linear Regression for Sensor Offset")
plt.show()

print(f"Quadratic Equation: O = {coeffs[0]:.5f}T² + {coeffs[1]:.5f}T + {coeffs[2]:.5f}")