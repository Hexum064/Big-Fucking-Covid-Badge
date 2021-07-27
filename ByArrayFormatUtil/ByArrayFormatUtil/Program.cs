using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ByArrayFormatUtil
{
    class Program
    {
        static void Main(string[] args)
        {
            string input;
            string data;
            string byteStr;
            string output = string.Empty;
            int index = 0;
            bool reverse = true;
            bool includeNewLine = true;

            Console.WriteLine("Input file.");
            input = Console.ReadLine();

            if (!File.Exists(input))
            {
                Console.WriteLine($"{input} not found.");
                Console.ReadLine();
                return;
            }

            data = File.ReadAllText(input);


            data = data.Replace(" ", "").Replace("\r", "").Replace("\n", "");

            while (index < data.Length - 7)
            {
                byteStr = data.Substring(index, 8);

                if (reverse)
                {
                    byteStr = new string(byteStr.Reverse().ToArray());
                }

                output += "0b" + byteStr + ",";

                if (includeNewLine)
                {
                    output += "\n";
                }

                index += 8;
            }

            Console.WriteLine("Output file.");
            input = Console.ReadLine();

            File.WriteAllText(input, output);
        }
    }
}
