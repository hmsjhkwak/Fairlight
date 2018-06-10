#include "Core/Core.h"
#include "File.h"
#include "Math/Math.h"

namespace util
{
namespace file
{
    std::string readAllText(const char* fileName)
    {
        FILE* f;
        fopen_s(&f, fileName, "rb");

        if (f == nullptr) { return ""; }

        // Determine file size
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);

        std::string text;
        text.resize(size + 1);

        rewind(f);
        fread(&text[0], sizeof(char), size, f);
        text[size] = '\0';
        fclose(f);

        return text;
    }

    void writeAllText(const char* fileName, const char* text)
    {
        FILE* f = fopen(fileName, "wb");
        fwrite(text, strlen(text), 1, f);
        fclose(f);
    }
}
}
