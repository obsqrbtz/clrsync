#include "file_browser.hpp"
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <cstring>

namespace file_dialogs {

std::string open_file_dialog(const std::string& title, 
                             const std::string& initial_path,
                             const std::vector<std::string>& filters) {
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
    
    if (!initial_path.empty() && std::filesystem::exists(initial_path)) {
        std::filesystem::path p(initial_path);
        if (std::filesystem::is_directory(p)) {
            ofn.lpstrInitialDir = initial_path.c_str();
        } else {
            std::string dir = p.parent_path().string();
            std::string name = p.filename().string();
            ofn.lpstrInitialDir = dir.c_str();
            strncpy(file, name.c_str(), sizeof(file) - 1);
        }
    }
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(file);
    }
    return "";
}

std::string save_file_dialog(const std::string& title,
                             const std::string& initial_path,
                             const std::vector<std::string>& filters) {
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
    
    if (!initial_path.empty()) {
        std::filesystem::path p(initial_path);
        if (std::filesystem::exists(p) && std::filesystem::is_directory(p)) {
            ofn.lpstrInitialDir = initial_path.c_str();
        } else {
            std::string dir = p.parent_path().string();
            std::string name = p.filename().string();
            if (std::filesystem::exists(dir)) {
                ofn.lpstrInitialDir = dir.c_str();
                strncpy(file, name.c_str(), sizeof(file) - 1);
            }
        }
    }
    
    if (GetSaveFileNameA(&ofn)) {
        return std::string(file);
    }
    return "";
}

std::string select_folder_dialog(const std::string& title,
                                 const std::string& initial_path) {
    IFileOpenDialog *pFileOpen;
    
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                                  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    
    if (SUCCEEDED(hr)) {
        DWORD dwFlags;
        if (SUCCEEDED(pFileOpen->GetOptions(&dwFlags))) {
            pFileOpen->SetOptions(dwFlags | FOS_PICKFOLDERS);
        }
        
        std::wstring wtitle(title.begin(), title.end());
        pFileOpen->SetTitle(wtitle.c_str());
        
        if (!initial_path.empty() && std::filesystem::exists(initial_path)) {
            IShellItem *psi = NULL;
            std::wstring winitial(initial_path.begin(), initial_path.end());
            hr = SHCreateItemFromParsingName(winitial.c_str(), NULL, IID_IShellItem, (void**)&psi);
            if (SUCCEEDED(hr)) {
                pFileOpen->SetFolder(psi);
                psi->Release();
            }
        }
        
        hr = pFileOpen->Show(GetActiveWindow());
        
        if (SUCCEEDED(hr)) {
            IShellItem *pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                
                if (SUCCEEDED(hr)) {
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

}

#else

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>

namespace file_dialogs {

std::string open_file_dialog(const std::string& title, 
                             const std::string& initial_path,
                             const std::vector<std::string>& filters) {
    @autoreleasepool {
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setTitle:[NSString stringWithUTF8String:title.c_str()]];
        [panel setCanChooseFiles:YES];
        [panel setCanChooseDirectories:NO];
        [panel setAllowsMultipleSelection:NO];
        
        if (!initial_path.empty()) {
            NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:initial_path.c_str()]];
            [panel setDirectoryURL:url];
        }
        
        if ([panel runModal] == NSModalResponseOK) {
            NSURL* url = [[panel URLs] objectAtIndex:0];
            return std::string([[url path] UTF8String]);
        }
    }
    return "";
}

std::string save_file_dialog(const std::string& title,
                             const std::string& initial_path,
                             const std::vector<std::string>& filters) {
    @autoreleasepool {
        NSSavePanel* panel = [NSSavePanel savePanel];
        [panel setTitle:[NSString stringWithUTF8String:title.c_str()]];
        
        if (!initial_path.empty()) {
            std::filesystem::path p(initial_path);
            if (std::filesystem::exists(p.parent_path())) {
                NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:p.parent_path().c_str()]];
                [panel setDirectoryURL:url];
                [panel setNameFieldStringValue:[NSString stringWithUTF8String:p.filename().c_str()]];
            }
        }
        
        if ([panel runModal] == NSModalResponseOK) {
            NSURL* url = [panel URL];
            return std::string([[url path] UTF8String]);
        }
    }
    return "";
}

std::string select_folder_dialog(const std::string& title,
                                 const std::string& initial_path) {
    @autoreleasepool {
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        [panel setTitle:[NSString stringWithUTF8String:title.c_str()]];
        [panel setCanChooseFiles:NO];
        [panel setCanChooseDirectories:YES];
        [panel setAllowsMultipleSelection:NO];
        
        if (!initial_path.empty()) {
            NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:initial_path.c_str()]];
            [panel setDirectoryURL:url];
        }
        
        if ([panel runModal] == NSModalResponseOK) {
            NSURL* url = [[panel URLs] objectAtIndex:0];
            return std::string([[url path] UTF8String]);
        }
    }
    return "";
}

}

#else

#include <gtk/gtk.h>

namespace file_dialogs {

std::string open_file_dialog(const std::string& title, 
                             const std::string& initial_path,
                             const std::vector<std::string>& filters) {
    if (!gtk_init_check(nullptr, nullptr)) {
        return "";
    }
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        title.c_str(),
        nullptr,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open", GTK_RESPONSE_ACCEPT,
        nullptr);
    
    if (!initial_path.empty()) {
        std::filesystem::path p(initial_path);
        if (std::filesystem::exists(p)) {
            if (std::filesystem::is_directory(p)) {
                gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_path.c_str());
            } else {
                gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), p.parent_path().c_str());
                gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), p.filename().c_str());
            }
        }
    }
    
    std::string result;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            result = filename;
            g_free(filename);
        }
    }
    
    gtk_widget_destroy(dialog);
    return result;
}

std::string save_file_dialog(const std::string& title,
                             const std::string& initial_path,
                             const std::vector<std::string>& filters) {
    if (!gtk_init_check(nullptr, nullptr)) {
        return "";
    }
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        title.c_str(),
        nullptr,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save", GTK_RESPONSE_ACCEPT,
        nullptr);
    
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    
    if (!initial_path.empty()) {
        std::filesystem::path p(initial_path);
        if (std::filesystem::exists(p.parent_path())) {
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), p.parent_path().c_str());
            gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), p.filename().c_str());
        }
    }
    
    std::string result;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            result = filename;
            g_free(filename);
        }
    }
    
    gtk_widget_destroy(dialog);
    return result;
}

std::string select_folder_dialog(const std::string& title,
                                 const std::string& initial_path) {
    if (!gtk_init_check(nullptr, nullptr)) {
        return "";
    }
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        title.c_str(),
        nullptr,
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Select", GTK_RESPONSE_ACCEPT,
        nullptr);
    
    if (!initial_path.empty() && std::filesystem::exists(initial_path)) {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_path.c_str());
    }
    
    std::string result;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            result = filename;
            g_free(filename);
        }
    }
    
    gtk_widget_destroy(dialog);
    return result;
}

}

#endif

#endif