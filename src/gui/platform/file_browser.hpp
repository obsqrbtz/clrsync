#ifndef CLRSYNC_GUI_FILE_BROWSER_HPP
#define CLRSYNC_GUI_FILE_BROWSER_HPP

#include <string>
#include <vector>

namespace file_dialogs
{
std::string open_file_dialog(const std::string &title = "Open File",
                             const std::string &initial_path = "",
                             const std::vector<std::string> &filters = {});

std::string save_file_dialog(const std::string &title = "Save File",
                             const std::string &initial_path = "",
                             const std::vector<std::string> &filters = {});

std::string select_folder_dialog(const std::string &title = "Select Folder",
                                 const std::string &initial_path = "");
} // namespace file_dialogs

#endif // CLRSYNC_GUI_FILE_BROWSER_HPP