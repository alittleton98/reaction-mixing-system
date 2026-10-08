using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace ReactionMixingSystem
{
    /// <summary>
    /// Interaction logic for SplashScreen.xaml
    /// </summary>
    public partial class SplashScreen : Window
    {
        static readonly TimeSpan NameDuration = TimeSpan.FromSeconds(3);
        static readonly TimeSpan SplashDuration = TimeSpan.FromSeconds(10); // total time on screen

        private const string splashScreenName = "pack://application:,,,/Resources/splash_name.png";
        private const string splashScreenBlank = "pack://application:,,,/Resources/splash_blank.png";

        /// <summary>
        /// Raised once, on the UI thread, when the splash has finished its sequence.
        /// </summary>
        public event EventHandler? Completed;

        public SplashScreen()
        {
            InitializeComponent();
            splashStatus.Text = "Starting...";
            splashVersion.Text = $"v{BuildDefines.RmdVersionString}";
            splashScreen.Source = new BitmapImage(new Uri(splashScreenName));
            Loaded += async (_, _) => await RunAsync();
        }

        // Name image for NameDuration, then the blank image for the rest of SplashDuration; then signal completion.
        private async Task InitializeModules()
        {
            try
            {
                await Task.Delay(NameDuration);
                //splashScreen.Source = new BitmapImage(new Uri(splashScreenBlank));
                await Task.Delay(SplashDuration);
            }
            finally
            {
                // Always signal, even if an image failed to load, so App never waits forever.
                Completed?.Invoke(this, EventArgs.Empty);
            }
        }

        public void SetStatus(string text)
        {
            splashStatus.Text = text;
        }

        protected override void OnInitialized(EventArgs args)
        {
            base.OnInitialized(args);

        }



    }
}
