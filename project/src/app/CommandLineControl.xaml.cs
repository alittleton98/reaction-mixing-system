using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;

namespace NexusMixingSystem;

/// <summary>
/// Read-only console-style view of everything written to stdout by the native libraries or the C# app.
/// </summary>
public partial class CommandLineControl : UserControl
{
    private const int MaxLines = 5000;

    private readonly object pendingLock = new();
    private readonly List<string> pending = new();
    private bool flushScheduled;
    private int lineCount;

    public CommandLineControl()
    {
        InitializeComponent();

        Loaded += (_, _) =>
        {
            StdoutCapture.LineWritten += OnLineWritten;
            foreach (string line in StdoutCapture.GetHistory())
            {
                Append(line);
            }
        };
        Unloaded += (_, _) => StdoutCapture.LineWritten -= OnLineWritten;
    }

    public void Append(string line)
    {
        OnLineWritten(line);
    }

    public void Clear()
    {
        OutputBox.Clear();
        lineCount = 0;
    }

    private void OnLineWritten(string line)
    {
        lock (pendingLock)
        {
            pending.Add(line);
            if (flushScheduled)
            {
                return;
            }
            flushScheduled = true;
        }
        Dispatcher.BeginInvoke(DispatcherPriority.Background, Flush);
    }

    private void Flush()
    {
        string[] lines;
        lock (pendingLock)
        {
            lines = pending.ToArray();
            pending.Clear();
            flushScheduled = false;
        }

        var sb = new StringBuilder();
        foreach (string line in lines)
        {
            sb.Append(line).Append('\n');
        }
        OutputBox.AppendText(sb.ToString());
        lineCount += lines.Length;

        if (lineCount > MaxLines)
        {
            int remove = lineCount - MaxLines;
            int idx = 0;
            for (int i = 0; i < remove; i++)
            {
                idx = OutputBox.Text.IndexOf('\n', idx) + 1;
            }
            OutputBox.Text = OutputBox.Text.Substring(idx);
            lineCount = MaxLines;
        }

        OutputBox.ScrollToEnd();
    }

    private void ClearButton_Click(object sender, RoutedEventArgs e)
    {
        Clear();
    }
}
