using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EDSNET;
using System.Threading;

namespace TestMultiThreadNET
{
    class Program
    {
        static Dictionary<string, string> state = new Dictionary<string, string>();
        static string[] colors1 = {"red", "blue"};
        static string[] colors2 = { "yellow", "blue" };
        static List<KeyValuePair<string, string>[]> testValues = new List<KeyValuePair<string,string>[]>();
        static int TESTCNT = 100;
        //regular collections are not threadsafe to append, use concurrent
        static System.Collections.Concurrent.ConcurrentQueue<TraceMessage> _trace = new System.Collections.Concurrent.ConcurrentQueue<TraceMessage>();
        static Random r;

        static void Main(string[] args)
        {
            string filename = "test_project.gz";
            string path = "..\\..\\..\\..\\..\\EDSEngine\\EDSEngineTestApp\\" + filename;
            EDSEngine knowledge = new EDSEngine(path);
            if (knowledge.IsOpen() == false)
            {
                knowledge = new EDSEngine(filename);
                if (knowledge.IsOpen() == false)
                {
                    write_result("FAILURE: Could not open rules xml file: " + path);
                    Quit();
                }
            }

            if (knowledge.IsOpen() == true)
                write_result("OK: File opened");

            knowledge.InputGetterDelegate = get_value;

            r = new Random();
            for (int cnt = 0; cnt < TESTCNT; cnt++)
            {
                string color1 = colors1[r.Next(0, 2)];
                string color2 = colors2[r.Next(0, 2)];
                testValues.Add(new KeyValuePair<string, string> [2] { new KeyValuePair<string, string>("Color1", color1), new KeyValuePair<string, string>("Color2", color2) });
            }

            //single threaded result           
            TheadStartObject singleThreadedParams = new TheadStartObject() { Rules = knowledge, Messages = _trace };
            TestCycle(singleThreadedParams);
            System.Collections.Concurrent.ConcurrentQueue<string> singleThreadedResults = singleThreadedParams.Results;

            Thread t1 = new Thread(new ParameterizedThreadStart(TestCycle));
            Thread t2 = new Thread(new ParameterizedThreadStart(TestCycle));
            Thread t3 = new Thread(new ParameterizedThreadStart(TestCycle));
            
            TheadStartObject threadParams1 = new TheadStartObject() { Rules = knowledge, Messages = _trace };
            TheadStartObject threadParams2 = new TheadStartObject() { Rules = knowledge, Messages = _trace };
            TheadStartObject threadParams3 = new TheadStartObject() { Rules = knowledge, Messages = _trace };

            t1.Start(threadParams1);
            t2.Start(threadParams2);
            t3.Start(threadParams3);
            t1.Join();            
            t2.Join();            
            t3.Join();

            //verify all the results are the same
            var equal1 = singleThreadedParams.Results.SequenceEqual<string>(threadParams1.Results);
            var equal2 = singleThreadedParams.Results.SequenceEqual<string>(threadParams2.Results);
            var equal3 = singleThreadedParams.Results.SequenceEqual<string>(threadParams3.Results);

            var equal = equal1 == true && equal2 == true && equal3 == true;

            write_result("All tests passed: " + equal.ToString());
            write_result("Press 't' key to show trace");
            ConsoleKeyInfo keyInfo = Console.ReadKey();
            if (keyInfo.KeyChar == 't')
                write_trace();
            Quit();
        }

        static void TestCycle(object obj)
        {
            EDSEngine knowledge = ((TheadStartObject)obj).Rules;
            System.Collections.Concurrent.ConcurrentQueue<TraceMessage> messages = ((TheadStartObject)obj).Messages;
            System.Collections.Concurrent.ConcurrentQueue<string> results = ((TheadStartObject)obj).Results;
            for (int cnt = 0; cnt < TESTCNT; cnt++)
            {
                ConsoleColor color = (ConsoleColor)r.Next(1, 16); //randomly pick any color but black to show this evaluation start/completion
                string msg1 = Thread.CurrentThread.ManagedThreadId.ToString() + " - begun " + cnt.ToString() + ": " + testValues[cnt][0].Value + "," + testValues[cnt][1].Value;
                messages.Enqueue(new TraceMessage() { Message = msg1, Color = color });
                string retval = "";
                
                Dictionary<string, string> thisState = new Dictionary<string, string>();
                thisState.Add(testValues[cnt][0].Key, testValues[cnt][0].Value);
                thisState.Add(testValues[cnt][1].Key, testValues[cnt][1].Value);

                retval = knowledge.GetFirstTableResult("ReverseTest", "OutColor", thisState);
                
                string msg2 = Thread.CurrentThread.ManagedThreadId.ToString() + " - complete " + cnt.ToString() + ": " + retval;
                messages.Enqueue(new TraceMessage() { Message = msg2, Color = color });
                results.Enqueue(retval);
                //write_result(msg);                    
                
            }
        }

        static void write_result(string s)
        {
            Console.WriteLine(s);
        }

        static void write_debug(string s)
        {
            Console.WriteLine("DEBUGGING: " + s);
        }

        static void write_trace()
        {
            foreach (TraceMessage item in _trace)
            {
                Console.ForegroundColor = item.Color;
                Console.WriteLine(item.Message);
            }
        }

        static string get_value(string name, object context)
        {
            Dictionary<string, string> state = (Dictionary<string, string>)context;
            if (state.ContainsKey(name))
                return state[name];
            else
                return "";            
        }



        static void Quit()
        {
            write_result("Press any key to quit");
            Console.ReadKey(true);
            return;
        }
    }

    public struct TraceMessage
    {
        public ConsoleColor Color;
        public string Message;
    }

    class TheadStartObject
    {
        public System.Collections.Concurrent.ConcurrentQueue<TraceMessage> Messages;
        public EDSEngine Rules;
        public System.Collections.Concurrent.ConcurrentQueue<string> Results = new System.Collections.Concurrent.ConcurrentQueue<string>();
    }
}
