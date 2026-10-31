using System.Windows;
using System.Runtime.CompilerServices;


internal static partial class NMS_Utilities
{
    public static void DisplayLog(string message, [CallerMemberName] string caller = "")
    {
        Console.WriteLine($"> {caller} | {message}");
    }

    public static void WarningLog(string message, [CallerMemberName] string caller = "")
    {
        Console.WriteLine($"> WARNING: {caller} | {message}");
    }

    public static void ErrorLog(string message, [CallerMemberName] string caller = "")
    {
        Console.WriteLine($"> ERROR: {caller} | {message}");
        MessageBox.Show($"> ERROR: {caller} | {message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
    }

    public static void FatalErrorLog(string message, bool ShouldThrowMessage = true, [CallerMemberName] string caller = "")
    {
        Console.WriteLine($"> ERROR: {caller} | {message}");
        if (ShouldThrowMessage)
        {
            MessageBoxResult result = MessageBox.Show($"> ERROR: {caller} | {message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            if (result == MessageBoxResult.OK)
            {
                Environment.Exit(1);
            }
        }
    }
}