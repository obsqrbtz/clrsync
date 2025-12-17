#include "file_browser.hpp"
#include <filesystem>

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

#endif