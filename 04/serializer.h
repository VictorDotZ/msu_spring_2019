#pragma once

enum class Error
{
    NoError,
    CorruptedArchive
};


class Serializer
{
public:
    explicit Serializer(std::ostream& out) : out_(out) {}

    template <class T>
    Error save(T& object) {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args) {
        return process(std::forward<ArgsT>(args)...);
    }

private:
    // process использует variadic templates
    template <class T>
    Error process(T&& value) {
        print(std::forward<T>(value));
        return Error::NoError;
    }

    template <class T, class... Args>
    Error process(T&& value, Args&&... args) {
        print(std::forward<T>(value));
        out_ << Separator;
        return process(std::forward<Args>(args)...);
    }

    void print(const uint64_t& value) {
        out_ << value;
    }

    void print(const bool& value) {
        out_ << (value ? "true" : "false");
    }

    std::ostream& out_;
    static constexpr char Separator = ' ';
};

class Deserializer
{
public:
    explicit Deserializer(std::istream& in) : in_(in) {}

    template <class T>
    Error load(T& object) {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT&&... args) {
        return process(std::forward<ArgsT>(args)...);
    }

private:
    template <class T>
    Error process(T&& value) {
        if (load(std::forward<T>(value)) == Error::CorruptedArchive)
            return Error::CorruptedArchive;
        return Error::NoError;
    }

    template <class T, class... Args>
    Error process(T&& value, Args&&... args) {
        if (load(std::forward<T>(value)) == Error::CorruptedArchive)
            return Error::CorruptedArchive;
        if (process(std::forward<Args>(args)...) == Error::CorruptedArchive)
            return Error::CorruptedArchive;
        return Error::NoError;
    }

    Error load(uint64_t& value) {
        std::string uintFieldAsText;
        in_ >> uintFieldAsText;
        if (uintFieldAsText[0] == '-')
            return Error::CorruptedArchive;
        try {
            value = std::stoull(uintFieldAsText);
        }
        catch (std::invalid_argument&) {
            return Error::CorruptedArchive;
        }
        return Error::NoError;
    }

    Error load(bool& value) {
        std::string boolFieldAsText;
        in_ >> boolFieldAsText;
        if (boolFieldAsText == "true")
            value = true;
        else if (boolFieldAsText == "false")
            value = false;
        else
            return Error::CorruptedArchive;
        return Error::NoError;
    }

    std::istream& in_;
};


