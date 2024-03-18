## Steps
Tested with Visual Studio 17 2022.
1. Clone the repository:
   ```
   git clone https://github.com/arkadva/anti-cheat-testing-tool.git
   ```
2. Navigate to the cloned directory:
   ```
   cd anti-cheat-testing-tool
   ```
3. Generate the project files:
   ```
   cmake -S . -B build -G "Visual Studio 17 2022"
   ```
Replace `Visual Studio 17 2022` with your installed Visual Studio version if different.

4. Build the project:
   ```
   cmake --build build --config Release
   ```

The executable `actt.exe` will be located in the `build/Release` directory.
