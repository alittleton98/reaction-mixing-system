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

namespace ReactionMixingSystem;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    //private const string icon = "pack://application:,,,/Resources/sound-wave-color.ico";
    private const string rmsCommonLibrary = "rms_common.dll";

    [DllImport(rmsCommonLibrary, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.U1)]
    private static extern bool ReactionCommon_Initialize();

    [DllImport(rmsCommonLibrary, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.U1)]
    private static extern bool ReactionCommon_Deinitialize();

    public MainWindow()
    {
        InitializeComponent();
        SourceInitialized += (_, _) => DarkTitleBar.Apply(this);
    }

    public void Button_Click(object sender, RoutedEventArgs e)
    {
        bool check = ReactionCommon_Initialize();
        CommonStatus.SetStatus(check ? "Initialized" : "Initialization failed");
        if (!check)
        {
            MessageBox.Show("Failed to initialize the Reaction Common Library.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        else
        {
            MessageBox.Show("Successfully initialized the Reaction Common Library.", "Display", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }
}