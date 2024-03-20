#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <vector>
#include <string>

namespace utilities {
  DWORD GetPIDByName(const std::wstring& name);

  template<typename StringType>
  HANDLE GetHandleByName(const std::wstring& name, BOOL inherit_handle, DWORD desired_access);

  std::vector<void*> GetVMTFunctions(void** vtable);

  // pe utilities
  PIMAGE_EXPORT_DIRECTORY GetExportDirectory(HMODULE module);
  PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(HMODULE module);
  PIMAGE_NT_HEADERS GetNTHeaders(HMODULE module);
}

#endif
