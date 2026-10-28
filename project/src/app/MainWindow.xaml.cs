using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace NexusMixingSystem;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    //private const string icon = "pack://application:,,,/Resources/sound-wave-color.ico";
    private const string nmsCommonLibrary = "nms_common.dll";

    [DllImport(nmsCommonLibrary, CallingConvention = CallingConvention.Cdecl, EntryPoint = NexusCommon_Initialize)]
    [return: MarshalAs(UnmanagedType.U1)]
    private static partial bool NexusCommon_Initialize();

    [DllImport(nmsCommonLibrary, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.U1)]
    private static extern bool NexusCommon_Deinitialize();

    [DllImport(nmsCommonLibrary, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.U1)]
    private static extern bool NexusCommon_SanityCheckScripting();

    public MainWindow()
    {
        InitializeComponent();
        SourceInitialized += (_, _) => DarkTitleBar.Apply(this);
    }

    public void Button_Click_Init(object sender, RoutedEventArgs e)
    {
        bool check = NexusCommon_Initialize();
        CommonStatus.SetStatus(check ? "Initialized" : "Initialization failed");
        if (!check)
        {
            MessageBox.Show("Failed to initialize the Nexus Common Library.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        else
        {
            MessageBox.Show("Successfully initialized the Nexus Common Library.", "Display", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }

    public void Button_Click_Script(object sender, RoutedEventArgs e)
    {
        bool check = NexusCommon_SanityCheckScripting();
        CommonStatus.SetStatus(check ? "Initialized" : "Initialization failed");
        if (!check)
        {
            MessageBox.Show("Failed to initialize the Nexus Common Library.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        else
        {
            MessageBox.Show("Successfully initialized the Nexus Common Library.", "Display", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }
}