#ifndef CLRSYNC_CORE_ERROR_HPP
#define CLRSYNC_CORE_ERROR_HPP

#include <optional>
#include <string>
#include <variant>

namespace clrsync::core
{

enum class error_code
{
    unknown,

    file_not_found,
    file_open_failed,
    file_write_failed,
    file_read_failed,
    dir_create_failed,

    parse_failed,
    invalid_format,

    config_missing,
    config_invalid,

    template_not_found,
    template_load_failed,
    template_apply_failed,

    palette_not_found,
    palette_load_failed,

    init_failed,
    invalid_arg,
    resource_missing,
};

inline const char *error_code_string(error_code code)
{
    switch (code)
    {
    case error_code::unknown:
        return "Unknown error";
    case error_code::file_not_found:
        return "File not found";
    case error_code::file_open_failed:
        return "Failed to open file";
    case error_code::file_write_failed:
        return "Failed to write file";
    case error_code::file_read_failed:
        return "Failed to read file";
    case error_code::dir_create_failed:
        return "Failed to create directory";
    case error_code::parse_failed:
        return "Parse failed";
    case error_code::invalid_format:
        return "Invalid format";
    case error_code::config_missing:
        return "Configuration missing";
    case error_code::config_invalid:
        return "Configuration invalid";
    case error_code::template_not_found:
        return "Template not found";
    case error_code::template_load_failed:
        return "Failed to load template";
    case error_code::template_apply_failed:
        return "Failed to apply template";
    case error_code::palette_not_found:
        return "Palette not found";
    case error_code::palette_load_failed:
        return "Failed to load palette";
    case error_code::init_failed:
        return "Initialization failed";
    case error_code::invalid_arg:
        return "Invalid argument";
    case error_code::resource_missing:
        return "Resource missing";
    default:
        return "Unknown error code";
    }
}

struct Error
{
    error_code code;
    std::string message;
    std::string context;

    Error(error_code c) : code(c), message(error_code_string(c))
    {
    }

    Error(error_code c, std::string msg) : code(c), message(std::move(msg))
    {
    }

    Error(error_code c, std::string msg, std::string ctx)
        : code(c), message(std::move(msg)), context(std::move(ctx))
    {
    }

    std::string description() const
    {
        if (context.empty())
            return message;
        return message + " [" + context + "]";
    }
};

template <typename T> class [[nodiscard]] Result
{
  private:
    std::variant<T, Error> m_data;

  public:
    Result(T value) : m_data(std::move(value))
    {
    }

    Result(Error error) : m_data(std::move(error))
    {
    }

    bool is_ok() const
    {
        return std::holds_alternative<T>(m_data);
    }

    bool is_error() const
    {
        return std::holds_alternative<Error>(m_data);
    }

    explicit operator bool() const
    {
        return is_ok();
    }

    T &value() &
    {
        return std::get<T>(m_data);
    }
    const T &value() const &
    {
        return std::get<T>(m_data);
    }
    T &&value() &&
    {
        return std::get<T>(std::move(m_data));
    }

    const Error &error() const
    {
        return std::get<Error>(m_data);
    }

    T value_or(T default_value) const
    {
        return is_ok() ? std::get<T>(m_data) : std::move(default_value);
    }

    std::optional<T> ok() const
    {
        if (is_ok())
            return std::get<T>(m_data);
        return std::nullopt;
    }

    std::optional<Error> err() const
    {
        if (is_error())
            return std::get<Error>(m_data);
        return std::nullopt;
    }

    template <typename F> auto map(F &&func) -> Result<decltype(func(std::declval<T>()))>
    {
        using U = decltype(func(std::declval<T>()));
        if (is_ok())
            return Result<U>(func(std::get<T>(m_data)));
        return Result<U>(std::get<Error>(m_data));
    }

    template <typename F> auto and_then(F &&func) -> decltype(func(std::declval<T>()))
    {
        if (is_ok())
            return func(std::get<T>(m_data));
        using ResultType = decltype(func(std::declval<T>()));
        return ResultType(std::get<Error>(m_data));
    }
};

template <> class [[nodiscard]] Result<void>
{
  private:
    std::optional<Error> m_error;

  public:
    Result() : m_error(std::nullopt)
    {
    }

    Result(Error error) : m_error(std::move(error))
    {
    }

    bool is_ok() const
    {
        return !m_error.has_value();
    }

    bool is_error() const
    {
        return m_error.has_value();
    }

    explicit operator bool() const
    {
        return is_ok();
    }

    const Error &error() const
    {
        return *m_error;
    }

    std::optional<Error> err() const
    {
        return m_error;
    }
};

template <typename T> Result<T> Ok(T value)
{
    return Result<T>(std::move(value));
}

inline Result<void> Ok()
{
    return Result<void>();
}

template <typename T> Result<T> Err(Error error)
{
    return Result<T>(std::move(error));
}

template <typename T> Result<T> Err(error_code code)
{
    return Result<T>(Error(code));
}

template <typename T> Result<T> Err(error_code code, std::string message)
{
    return Result<T>(Error(code, std::move(message)));
}

template <typename T> Result<T> Err(error_code code, std::string message, std::string context)
{
    return Result<T>(Error(code, std::move(message), std::move(context)));
}

} // namespace clrsync::core

#endif // CLRSYNC_CORE_ERROR_HPP
