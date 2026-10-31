using System.IO;
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

    public MainWindow()
    {
        InitializeComponent();
        SourceInitialized += (_, _) => DarkTitleBar.Apply(this);

    }

    public void Button_Click_Init(object sender, RoutedEventArgs e)
    {
        App.WriteLog("Testing");
        bool check = true;
        CommonStatus.SetStatus(check ? "Initialized" : "Initialization failed");
        NMS_NativeCode.NexusSoundEngine_InitializeSoundEngine();
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