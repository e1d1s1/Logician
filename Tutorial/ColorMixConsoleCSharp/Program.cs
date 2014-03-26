using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EDSNET;

namespace ColorMixConsoleCSharp
{
    class Program
    {
        static EDSEngine m_TableEvaluator = null;
        static Dictionary<string, string> mAppData = new Dictionary<string, string>();
        static void Main(string[] args)
        {
            Console.WriteLine("Loading the rules file ColorRules.xml...");
            m_TableEvaluator = new EDSEngine("ColorRules.xml");
            m_TableEvaluator.InputGetterDelegate = delegate(string attrName, object context)
            {
                if (mAppData.ContainsKey(attrName))
                    return mAppData[attrName];
                else
                    return "";
            };
            Console.WriteLine("done\n");

            Console.WriteLine("Enter red or blue for first paint color:");
            mAppData["PaintColor1"] = Console.ReadLine();
            Console.WriteLine("Enter yellow or blue for second paint color:");
            mAppData["PaintColor2"] = Console.ReadLine();

            Console.WriteLine("The result is: " + GetResultingColor() + "\n");

            pause();
        }

        static void pause()
        {
            Console.WriteLine("Press any key to exit");
            Console.ReadKey();
        }

        static string GetSingleSolution(string tableToEvaluate, string nameOfOutput) //could reuse this function for all similar aplication events
        {            
            string[] results = m_TableEvaluator.EvaluateTable(tableToEvaluate, nameOfOutput);
            //EDSEngine supports returning multiple true results on a sigle line, but in this case we just want a single result (the first one it finds)

            if (results.Length > 0)
                return results[0];
            else
                return ""; //shouldn't happen since we have a "fallout" rule at the end of the table for unspecified combos
        }

        static string GetResultingColor()
        {
            return GetSingleSolution("ColorMixingTable", "ResultColor");
        }
    }
}
