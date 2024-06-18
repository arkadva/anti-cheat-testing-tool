#include "./Base/cheatmanager.h"
#include "./Utils/implementation_imports.h"
#include "./Utils/pipeclient.h"
#include "./Utils/process.h"
#include "./Utils/debug.h"
#include "./Utils/utilities.h"

/*
void Demo1(CheatManager* cheatManager);
void Demo2(CheatManager* cheatManager);
void Demo3(CheatManager* cheatManager);
void Demo4(CheatManager* cheatManager);
int Demo5(CheatManager* cheatManager);
*/

int main() {
  const std::wstring process_name = L"D:\\AssaultCube 1.3.0.2\\bin_win32\\ac_client.exe";

  const std::wstring working_directory = L"D:\\AssaultCube 1.3.0.2";
  std::vector<std::wstring> args = {
    L"\"--home=?MYDOCUMENTS?\\My Games\\AssaultCube\\v1.3\"",
    L"--init %1 %2 %3 %4 %5"
  };

  Process* process = new Process(process_name, args, working_directory);

  /*
  const std::wstring process_name = L"ac_client.exe";
  Process* process = new Process(process_name, utilities::GetPIDByName(process_name));
  */

  CheatManager* cheatManager = new CheatManager(process);

  Sleep(2000);

  return 0;
}
