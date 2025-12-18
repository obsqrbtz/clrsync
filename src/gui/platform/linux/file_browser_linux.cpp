#ifdef __linux__

#include "gui/platform/file_browser.hpp"
#include <gtk/gtk.h>
#include <filesystem>

namespace file_dialogs
{

std::string open_file_dialog(const std::string &title, const std::string &initial_path,
                             const std::vector<std::string> &filters)
{
    if (!gtk_init_check(nullptr, nullptr))
    {
        return "";
    }

    GtkFileChooserNative *native = gtk_file_chooser_native_new(
        title.c_str(), nullptr, GTK_FILE_CHOOSER_ACTION_OPEN, "_Open", "_Cancel");

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);

    if (!initial_path.empty())
    {
        std::filesystem::path p(initial_path);
        if (std::filesystem::exists(p))
        {
            if (std::filesystem::is_directory(p))
            {
                gtk_file_chooser_set_current_folder(chooser, initial_path.c_str());
            }
            else
            {
                gtk_file_chooser_set_current_folder(chooser, p.parent_path().c_str());
                gtk_file_chooser_set_current_name(chooser, p.filename().c_str());
            }
        }
    }

    std::string result;
    if (gtk_native_dialog_run(GTK_NATIVE_DIALOG(native)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(chooser);
        if (filename)
        {
            result = filename;
            g_free(filename);
        }
    }

    g_object_unref(native);
    while (gtk_events_pending())
        gtk_main_iteration();
    return result;
}

std::string save_file_dialog(const std::string &title, const std::string &initial_path,
                             const std::vector<std::string> &filters)
{
    if (!gtk_init_check(nullptr, nullptr))
    {
        return "";
    }

    GtkFileChooserNative *native = gtk_file_chooser_native_new(
        title.c_str(), nullptr, GTK_FILE_CHOOSER_ACTION_SAVE, "_Save", "_Cancel");

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    if (!initial_path.empty())
    {
        std::filesystem::path p(initial_path);
        if (std::filesystem::exists(p.parent_path()))
        {
            gtk_file_chooser_set_current_folder(chooser, p.parent_path().c_str());
            gtk_file_chooser_set_current_name(chooser, p.filename().c_str());
        }
    }

    std::string result;
    if (gtk_native_dialog_run(GTK_NATIVE_DIALOG(native)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(chooser);
        if (filename)
        {
            result = filename;
            g_free(filename);
        }
    }

    g_object_unref(native);
    while (gtk_events_pending())
        gtk_main_iteration();
    return result;
}

std::string select_folder_dialog(const std::string &title, const std::string &initial_path)
{
    if (!gtk_init_check(nullptr, nullptr))
    {
        return "";
    }

    GtkFileChooserNative *native = gtk_file_chooser_native_new(
        title.c_str(), nullptr, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "_Select", "_Cancel");

    GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);

    if (!initial_path.empty() && std::filesystem::exists(initial_path))
    {
        gtk_file_chooser_set_current_folder(chooser, initial_path.c_str());
    }

    std::string result;
    if (gtk_native_dialog_run(GTK_NATIVE_DIALOG(native)) == GTK_RESPONSE_ACCEPT)
    {
        char *filename = gtk_file_chooser_get_filename(chooser);
        if (filename)
        {
            result = filename;
            g_free(filename);
        }
    }

    g_object_unref(native);
    while (gtk_events_pending())
        gtk_main_iteration();
    return result;
}

} // namespace file_dialogs
#endif