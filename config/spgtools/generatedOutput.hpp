/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#ifndef SPGTOOLS_GENERATED_OUTPUT_HPP
#define SPGTOOLS_GENERATED_OUTPUT_HPP

#include <sstream>
#include <vector>

/**
 * Generated output class.
 *
 * This supports << operations. Internally these go to a string stream,
 * and when we close the file it gets written to file if the contents
 * are different from what is on file already.
 */
class GeneratedOutput
{
public:
    // constructor
    //
    // this will initialise the stringstream buffer and the line count
    // it will not even attempt to open the file
    //
    // writing to file only occurs when the stream is implicitly destroyed
    // (by going out of scope) or explicitly closed
    //
    // note that cwd is from where we will be compiling the code
    GeneratedOutput(const std::string& filename,
                    const std::string& cwd);

    // in the version without cwd then we will be compiling the code from
    // the same directory as filename and cwd is effectively set to
    // dirname(filename)
    GeneratedOutput(const std::string& filename);

    ~GeneratedOutput();

    template<typename T>
    GeneratedOutput& operator << (const T& rhs)
    {
        testClosure();
        m_oss << rhs;
        return *this;
    }

    // however we will explicitly implement that puts anything which might
    // include new lines to the stream
    //
    // this is because we want to count the number of lines
    GeneratedOutput& operator << (char c);
    GeneratedOutput& operator << (const char* str);
    GeneratedOutput& operator << (const std::string& str);

    size_t lines() const;
    const std::string& filename() const;

    bool close();

    std::string relativePath(const std::string& filename);

private:
    std::string m_filename;
    std::string m_cwd;
    std::ostringstream m_oss;
    size_t m_lines;
    bool m_closed;

    void testClosure() const;

    GeneratedOutput(const GeneratedOutput&);
    GeneratedOutput& operator=(const GeneratedOutput&);
};

std::string headerGuardMacroName(const std::string& fn);

void writeStartCommentBlock(
    GeneratedOutput& ostr,
    bool doxygenate=false,
    size_t indent=0,
    size_t vIndent=1);
void writeEndCommentBlock(GeneratedOutput& ostr, size_t hIndent=0);
void writeComments(
    GeneratedOutput& ostr,
    const std::vector<std::string>& text,
    size_t indent=0,
    size_t hIndent=0);
void writeGeneratedCodeNotice(GeneratedOutput& ostr, const std::string& ofn);
void startHeaderFile(GeneratedOutput& ostr, const std::string& fn);
void endHeaderFile(GeneratedOutput& ostr, const std::string& fn);
void startSourceFile(GeneratedOutput& ostr, const std::string& fn,
    bool doxygenate=true);
void endSourceFile(GeneratedOutput& ostr, const std::string& fn);
void endSourceFile(GeneratedOutput& ostr, const std::string& fn);

/**
 * Writes newContents to the file filename if the new contents are different
 * from the existing contents.
 *
 * Returns true if it wrote the file.
 *
 * Throws an exception on failure to write when attempting to write.
 */
bool writeFileIfChanged(
    const char*        filename,
    const std::string& newContents);

#endif
