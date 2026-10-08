using System.Configuration;
using System.Data;
using System.Windows;
using System.Windows.Threading;

namespace ReactionMixingSystem;

/// <summary>
/// Interaction logic for App.xaml
/// </summary>
public partial class App : Application
{
    static readonly TimeSpan SplashDuration = TimeSpan.FromSeconds(10);
    
    public App()
    {
    }

    void Application_Startup(object sender, StartupEventArgs e)
    {
        ShutdownMode = ShutdownMode.OnExplicitShutdown;

        SplashScreen splash = new SplashScreen();
        splash.Show();

        //splash.

        var timer = new DispatcherTimer { Interval = SplashDuration };
        timer.Tick += (_, _) =>
        {
            timer.Stop();
            var main = new MainWindow();
            MainWindow = main;
            ShutdownMode = ShutdownMode.OnMainWindowClose;
            splash.Close();
            main.Show();
      
        };
        timer.Start();
    }
}



