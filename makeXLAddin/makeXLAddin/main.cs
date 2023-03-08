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
using System;

using Excel = Microsoft.Office.Interop.Excel;

namespace MakeXLAddin
{
    class Program
    {
        // Command line arguments for MakeXLAddin.
        class CommandLine
        {
            public bool m_visible = false;
            public string m_ofn = "";
            public string[] m_ifns;
            public string m_projectName = "";
            public Excel.XlFileFormat m_fileFormat;

            // Constructor of Args - parses the command line
            public CommandLine(string[] args)
            {
                bool getProjectName = false;
                m_visible = false;
                int i;
                for (i = 0; i < args.Length; ++i)
                {
                    string arg = args[i];
                    if (getProjectName)
                    {
                        m_projectName = args[i];
                        getProjectName = false;
                    }
                    else if (arg.StartsWith("-"))
                    {
                        string option = arg.Substring(1);
                        if (option == "v")
                            m_visible = true;
                        else if (option == "p")
                            getProjectName = true;
                        else
                            throw new ArgumentException("Bad option value", arg);
                    }
                    else
                        break;
                }

                if (i >= args.Length)
                    throw new ArgumentException("Output file not specified");

                m_ofn = args[i];
                ++i;

                string ext = System.IO.Path.GetExtension(m_ofn).ToLower();

                if (ext == ".xlam")
                    m_fileFormat = Excel.XlFileFormat.xlOpenXMLAddIn;
                else if (ext == ".xla")
                    m_fileFormat = Excel.XlFileFormat.xlAddIn;
                else
                    throw new ArgumentException("Unrecognised extension for output file - should be .xla or .xlam");

                if (i >= args.Length)
                    throw new ArgumentException("No import files specified");

                m_ifns = new string[args.Length - i];
                int j = 0;

                while (i < args.Length)
                {
                    m_ifns[j] = args[i];
                    ++i;
                    ++j;
                }
            }
        }

        static int Main(string[] args)
        {
            Console.WriteLine("MakeXLAddin");
            try
            {
                CommandLine cl = new CommandLine(args);

                Excel.Application xl = new Excel.Application();
                if (cl.m_visible)
                {
                    xl.Visible = true;
                }
                Excel.Workbook wb = xl.Workbooks.Add();
                wb.IsAddin = true;

                string ffn;
                for (int i = 0; i < cl.m_ifns.Length; ++i)
                {
                    ffn = System.IO.Path.GetFullPath(cl.m_ifns[i]);
                    Console.WriteLine("importing {0}", ffn);
                    wb.VBProject.VBComponents.Import(ffn);
                }

                if (cl.m_projectName.Length > 0)
                {
                    Console.WriteLine("setting project name to {0}", cl.m_projectName);
                    wb.VBProject.VBComponents.VBE.ActiveVBProject.Name = cl.m_projectName;
                }

                ffn = System.IO.Path.GetFullPath(cl.m_ofn);
                if (System.IO.File.Exists(ffn))
                {
                    Console.WriteLine("removing {0}", ffn);
                    System.IO.File.Delete(ffn);
                }

                Console.WriteLine("writing {0}", ffn);
                var missing = System.Reflection.Missing.Value;

                wb.SaveAs(ffn, 
                    cl.m_fileFormat,
                    "", "", false, false, 
                    Excel.XlSaveAsAccessMode.xlExclusive,
                    missing, missing, missing, missing, missing);

                if (!cl.m_visible)
                {
                    wb.Close(false, missing, missing);
                    xl.Quit();
                }
            }
            catch (System.Exception e)
            {
                Console.Error.WriteLine(e.Message);
                return -1;
            }

            return 0;
        }
    }
}
