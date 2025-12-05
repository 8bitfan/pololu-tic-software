/**
 * Example program demonstrating how to use libpololu-tic as a C++ DLL/shared library.
 *
 * This example shows:
 * - Enumerating connected Tic devices
 * - Opening a connection to a device
 * - Getting device information
 * - Reading current status
 * - Controlling the motor
 * - Proper error handling with exceptions
 *
 * Build with CMake or manually:
 *   g++ -std=c++11 example.cpp -lpololu-tic-1 -o example
 */

#include <tic.hpp>
#include <iostream>
#include <iomanip>
#include <vector>

// Function to display device information
void display_device_info(const tic::device & device)
{
  std::cout << "Device Information:" << std::endl;
  std::cout << "  Name: " << device.get_name() << std::endl;
  std::cout << "  Short Name: " << device.get_short_name() << std::endl;
  std::cout << "  Serial Number: " << device.get_serial_number() << std::endl;
  std::cout << "  Firmware Version: 0x" << std::hex << device.get_firmware_version() << std::dec << std::endl;
  std::cout << std::endl;
}

// Function to display current status variables
void display_status(const tic::variables & vars)
{
  std::cout << "Current Status:" << std::endl;
  std::cout << "  Operation State: " << (int)vars.get_operation_state() << std::endl;
  std::cout << "  Energized: " << (vars.get_energized() ? "Yes" : "No") << std::endl;
  std::cout << "  Position Uncertain: " << (vars.get_position_uncertain() ? "Yes" : "No") << std::endl;
  std::cout << "  Current Position: " << vars.get_current_position() << std::endl;
  std::cout << "  Target Position: " << vars.get_target_position() << std::endl;
  std::cout << "  Current Velocity: " << vars.get_current_velocity() << std::endl;
  std::cout << "  Target Velocity: " << vars.get_target_velocity() << std::endl;
  std::cout << "  VIN Voltage: " << vars.get_vin_voltage() / 1000.0 << " V" << std::endl;
  std::cout << "  Uptime: " << vars.get_up_time() << " ms" << std::endl;
  
  uint16_t error_status = vars.get_error_status();
  if (error_status)
  {
    std::cout << "  Errors Present: 0x" << std::hex << error_status << std::dec << std::endl;
  }
  else
  {
    std::cout << "  No Errors" << std::endl;
  }
  std::cout << std::endl;
}

// Example: Basic device enumeration and status reading
void example_enumerate_and_read_status()
{
  std::cout << "=== Example: Enumerate Devices and Read Status ===" << std::endl;
  
  // List all connected Tic devices
  std::vector<tic::device> devices = tic::list_connected_devices();
  
  if (devices.empty())
  {
    std::cout << "No Tic devices found." << std::endl;
    std::cout << "Please connect a Tic device and try again." << std::endl;
    return;
  }
  
  std::cout << "Found " << devices.size() << " Tic device(s)." << std::endl;
  std::cout << std::endl;
  
  // Connect to the first device
  tic::device device = devices[0];
  display_device_info(device);
  
  // Open a handle to communicate with the device
  tic::handle handle(device);
  
  // Get and display current status
  tic::variables vars = handle.get_variables();
  display_status(vars);
}

// Example: Motor control
void example_motor_control()
{
  std::cout << "=== Example: Motor Control ===" << std::endl;
  
  std::vector<tic::device> devices = tic::list_connected_devices();
  
  if (devices.empty())
  {
    std::cout << "No Tic devices found." << std::endl;
    return;
  }
  
  tic::handle handle(devices[0]);
  
  std::cout << "This example will move the motor to position 1000." << std::endl;
  std::cout << "Make sure it's safe to move the motor before proceeding!" << std::endl;
  std::cout << std::endl;
  std::cout << "WARNING: This will cause motor movement!" << std::endl;
  std::cout << "Type 'yes' to continue, or anything else to skip: ";
  
  std::string response;
  std::getline(std::cin, response);
  
  if (response != "yes")
  {
    std::cout << "Skipping motor control example." << std::endl;
    return;
  }
  
  // Exit safe start (required before motor will move)
  std::cout << "Exiting safe start..." << std::endl;
  handle.exit_safe_start();
  
  // Set target position
  int32_t target_position = 1000;
  std::cout << "Setting target position to " << target_position << "..." << std::endl;
  handle.set_target_position(target_position);
  
  // Read current status
  tic::variables vars = handle.get_variables();
  std::cout << "Current position: " << vars.get_current_position() << std::endl;
  std::cout << "Target position: " << vars.get_target_position() << std::endl;
  
  // Halt and hold the motor
  std::cout << "Halting motor..." << std::endl;
  handle.halt_and_hold();
  
  std::cout << "Motor control example complete." << std::endl;
  std::cout << std::endl;
}

// Example: Using the C API (instead of C++ wrapper)
void example_c_api()
{
  std::cout << "=== Example: Using C API from C++ ===" << std::endl;
  
  tic_device ** device_list;
  size_t device_count;
  tic_error * error;
  
  // List connected devices using C API
  error = tic_list_connected_devices(&device_list, &device_count);
  
  if (error)
  {
    std::cerr << "Error listing devices: " << tic_error_get_message(error) << std::endl;
    tic_error_free(error);
    return;
  }
  
  if (device_count == 0)
  {
    std::cout << "No Tic devices found." << std::endl;
    tic_list_free(device_list);
    return;
  }
  
  std::cout << "Found " << device_count << " device(s) using C API." << std::endl;
  
  // Get serial number of first device
  const char * serial_number = tic_device_get_serial_number(device_list[0]);
  std::cout << "First device serial number: " << serial_number << std::endl;
  
  // Open a handle
  tic_handle * handle;
  error = tic_handle_open(device_list[0], &handle);
  
  if (error)
  {
    std::cerr << "Error opening device: " << tic_error_get_message(error) << std::endl;
    tic_error_free(error);
    tic_list_free(device_list);
    return;
  }
  
  // Get variables
  tic_variables * vars;
  error = tic_get_variables(handle, &vars, false);
  
  if (error)
  {
    std::cerr << "Error getting variables: " << tic_error_get_message(error) << std::endl;
    tic_error_free(error);
    tic_handle_close(handle);
    tic_list_free(device_list);
    return;
  }
  
  // Display current position
  int32_t position = tic_variables_get_current_position(vars);
  std::cout << "Current position (from C API): " << position << std::endl;
  
  // Clean up
  tic_variables_free(vars);
  tic_handle_close(handle);
  tic_list_free(device_list);
  
  std::cout << std::endl;
}

int main(int argc, char ** argv)
{
  std::cout << "Pololu Tic C++ DLL/Shared Library Example" << std::endl;
  std::cout << "==========================================" << std::endl;
  std::cout << std::endl;
  
  try
  {
    // Example 1: Enumerate devices and read status
    example_enumerate_and_read_status();
    
    // Example 2: Motor control (requires user confirmation)
    example_motor_control();
    
    // Example 3: Using the C API from C++
    example_c_api();
    
    std::cout << "All examples completed successfully!" << std::endl;
  }
  catch (const tic::error & e)
  {
    std::cerr << "Tic Error: " << e.what() << std::endl;
    return 1;
  }
  catch (const std::exception & e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
