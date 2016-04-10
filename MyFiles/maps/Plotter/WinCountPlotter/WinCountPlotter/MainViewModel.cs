using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System;

using OxyPlot;
using OxyPlot.Series;
using System.IO;
using OxyPlot.Axes;

namespace WinCountPlotter
{
    class MainViewModel
    {

        const string spider500 = "win_count_sm_12_500.txt";
        const string spider1000 = "win_count_sm_12_1000.txt";
        const string normal500 = "win_count_500.txt";
        const string normal1000 = "win_count_1000.txt";
        const string benchmark500 = "win_count_500_benchmark.txt";
        const string benchmark1000 = "win_count_1000_benchmark.txt";
        const string folderName = "StatFiles";

        public struct PrintFlags
        {
            public const int None                   = 0;
            public const int SpiderMine500          = 1;
            public const int SpiderMine1000         = 1 << 1;
            public const int Normal500              = 1 << 2;
            public const int Normal1000             = 1 << 3;
            public const int BenchmarkPaper500      = 1 << 4;
            public const int BenchmarkPaper1000     = 1 << 5;
            public const int BenchmarkLicenta500    = 1 << 4;
            public const int BenchmarkLicenta1000   = 1 << 4;
            public const int All                    = (1 << 6) - 1;
        }

        static Dictionary<string, List<int>> values;
        static Dictionary<string, int> flagsMap;

        public MainViewModel()
        {

            values = new Dictionary<string, List<int>>();

            values.Add(spider500, new List<int>());
            values.Add(spider1000, new List<int>());
            values.Add(normal500, new List<int>());
            values.Add(normal1000, new List<int>());
            values.Add(benchmark500, new List<int>());
            values.Add(benchmark1000, new List<int>());

            flagsMap = new Dictionary<string, int>();

            flagsMap.Add(spider500, PrintFlags.SpiderMine500);
            flagsMap.Add(spider1000, PrintFlags.SpiderMine1000);
            flagsMap.Add(normal500, PrintFlags.Normal500);
            flagsMap.Add(normal1000, PrintFlags.Normal1000);
            flagsMap.Add(benchmark500, PrintFlags.BenchmarkPaper500);
            flagsMap.Add(benchmark1000, PrintFlags.BenchmarkPaper1000);


            ReadTheValues();

            this.MyModel = new PlotModel { Title = "Comparison on 500 episodes" , PlotType = PlotType.XY};
            this.MyModel.Axes.Add(new LinearAxis
            {
                IntervalLength = 10,
                MajorGridlineStyle = LineStyle.Solid,
                MajorGridlineColor = OxyColor.FromRgb(0, 0, 0),
                MinorGridlineStyle = LineStyle.LongDash,
                Unit = "%",
                MajorStep = 10,
                MinorStep = 5,
                Position = AxisPosition.Left,
                Minimum = 0,
                Maximum = 100,
            });


            this.MyModel.LegendPosition = LegendPosition.LeftTop;
            this.MyModel.LegendPlacement = LegendPlacement.Outside;

            plotAll(PrintFlags.BenchmarkPaper500 | PrintFlags.BenchmarkPaper1000);
        }

        public PlotModel MyModel { get; private set; }
        
        void plotAll(int flags)
        {
            for(int i = 0; i < values.Count; i++)
            {
                int flagValue = flagsMap.ElementAt(i).Value;

                if ((flagValue & flags) == 0)
                    continue;

                OxyPlot.Series.LineSeries puncte = new LineSeries();
                List<int> currentvalues = values.ElementAt(i).Value;

                puncte.Title = GetProperString(values.ElementAt(i).Key);

                int maxValues = (currentvalues.Count/500) * 500;

                for(int j = 0; j < maxValues; j++)
                {
                    puncte.Points.Add(new OxyPlot.DataPoint(j + 1, (float)values.ElementAt(i).Value[j] * 100f / (float)maxValues));
                }

                this.MyModel.Series.Add(puncte);
            }
        }

        static string GetProperString(string str)
        {
            if (str.Equals(spider1000) || str.Equals(spider500))
                return "A w/ SM";
            else if (str.Equals(normal1000) || str.Equals(normal500))
                return "Simple Agent";
            else if (str.Equals(benchmark1000))
                return "Benchmark 1000";
            else if (str.Equals(benchmark500))
                return "Benchmark 500";
            else
                return "Unknown";
        }

        static void ReadTheValues()
        {
            for (int i = 0; i < values.Count; i++)
            {
                string key = values.Keys.ToList<string>()[i];
                int total = 0;
                using (StreamReader sr = new StreamReader(key))
                {
                    string allText = sr.ReadToEnd();
                    string[] bits = allText.Split(' ');

                    for (int j = 0; j < bits.Length - 1; j++)
                    {
                        int point = Int32.Parse(bits[j]);
                        total += point;
                        values[key].Add(total);
                    }
                }
            }
        }
    }
}
