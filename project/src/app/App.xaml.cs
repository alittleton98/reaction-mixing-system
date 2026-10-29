using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Windows;
using System.Windows.Media.Animation;
using System.Windows.Threading;

namespace NexusMixingSystem;

/// <summary>
/// Interaction logic for App.xaml
/// </summary>
/// 


public partial class App : Application
{
    private SplashScreen splash;
    private const int LoadingScreenDelayBetweenLoads = 500;
    private string UserScriptPath = "";

    public App()
    {
        splash = new SplashScreen();
    }

    private async Task InitializeClient()
    {
        
    }

    private async Task InitializeSoundEngine()
    {
        if (splash == null) 
        {
            return;
        }
        splash.SetStatus("Sound Engine Init...");
        await Task.Run(() => NMS_NativeCode.NexusSoundEngine_InitializeSoundEngine());
        splash.SetStatus("Sound Engine Loaded!");

    }

    async void Application_Startup(object sender, StartupEventArgs e)
    {
        StdoutCapture.Start();
        ShutdownMode = ShutdownMode.OnExplicitShutdown;
        splash.Show();
        if (e.Args[0] == "-script" && e.Args.Length > 1)
        {
            UserScriptPath = e.Args[1]; 
        }
        else 
        {
            UserScriptPath = BuildDefines.ScriptOutputDirectory + BuildDefines.ScriptDefaultName + BuildDefines.ScriptFileExtension;
        }

        // Load core modules and data


        // Initialize feature modules
        await InitializeSoundEngine();
        await Task.Delay(LoadingScreenDelayBetweenLoads);

        // Ready main window
        var main = new MainWindow();
        MainWindow = main;
        ShutdownMode = ShutdownMode.OnMainWindowClose;
        
        // clean up splash
        splash.SetStatus("Done!");
        await Task.Delay(LoadingScreenDelayBetweenLoads);
        splash.Close();
        main.Show();
    }


}




