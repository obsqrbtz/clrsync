#ifdef _WIN32

#include "gui/platform/file_browser.hpp"
#include <filesystem>

#include <commdlg.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <windows.h>

namespace file_dialogs
{

std::string open_file_dialog(const std::string &title, const std::string &initial_path,
                             const std::vector<std::string> &filters)
{
    OPENFILENAMEA ofn;
    char file[MAX_PATH] = "";

    std::string filter_str = "All Files (*.*)\0*.*\0";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = file;
    ofn.nMaxFile = sizeof(file);
    ofn.lpstrFilter = filter_str.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (!initial_path.empty() && std::filesystem::exists(initial_path))
    {
        std::filesystem::path p(initial_path);
        if (std::filesystem::is_directory(p))
        {
            ofn.lpstrInitialDir = initial_path.c_str();
        }
        else
        {
            std::string dir = p.parent_path().string();
            std::string name = p.filename().string();
            ofn.lpstrInitialDir = dir.c_str();
            strncpy(file, name.c_str(), sizeof(file) - 1);
        }
    }

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(file);
    }
    return "";
}

std::string save_file_dialog(const std::string &title, const std::string &initial_path,
                             const std::vector<std::string> &filters)
{
    OPENFILENAMEA ofn;
    char file[MAX_PATH] = "";

    std::string filter_str = "All Files\0*.*\0\0";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = file;
    ofn.nMaxFile = sizeof(file);
    ofn.lpstrFilter = filter_str.c_str();
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (!initial_path.empty())
    {
        std::filesystem::path p(initial_path);
        if (std::filesystem::exists(p) && std::filesystem::is_directory(p))
        {
            ofn.lpstrInitialDir = initial_path.c_str();
        }
        else
        {
            std::string dir = p.parent_path().string();
            std::string name = p.filename().string();
            if (std::filesystem::exists(dir))
            {
                ofn.lpstrInitialDir = dir.c_str();
                strncpy(file, name.c_str(), sizeof(file) - 1);
            }
        }
    }

    if (GetSaveFileNameA(&ofn))
    {
        return std::string(file);
    }
    return "";
}

std::string select_folder_dialog(const std::string &title, const std::string &initial_path)
{
    IFileOpenDialog *pFileOpen;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog,
                                  reinterpret_cast<void **>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
        DWORD dwFlags;
        if (SUCCEEDED(pFileOpen->GetOptions(&dwFlags)))
        {
            pFileOpen->SetOptions(dwFlags | FOS_PICKFOLDERS);
        }

        std::wstring wtitle(title.begin(), title.end());
        pFileOpen->SetTitle(wtitle.c_str());

        if (!initial_path.empty() && std::filesystem::exists(initial_path))
        {
            IShellItem *psi = NULL;
            std::wstring winitial(initial_path.begin(), initial_path.end());
            hr = SHCreateItemFromParsingName(winitial.c_str(), NULL, IID_IShellItem, (void **)&psi);
            if (SUCCEEDED(hr))
            {
                pFileOpen->SetFolder(psi);
                psi->Release();
            }
        }

        hr = pFileOpen->Show(GetActiveWindow());

        if (SUCCEEDED(hr))
        {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr))
                {
                    std::wstring wpath(pszFilePath);
                    std::string result(wpath.begin(), wpath.end());
                    CoTaskMemFree(pszFilePath);
                    pItem->Release();
                    pFileOpen->Release();
                    return result;
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }

    return "";
}

} // namespace file_dialogs

#endif