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
        static readonly TimeSpan SplashDuration = TimeSpan.FromSeconds(3);

        private const string splashScreenName = "pack://application:,,,/Resources/splash_name.png";
        private const string splashScreenBlank = "pack://application:,,,/Resources/splash_blank.png";

        public SplashScreen()
        {
            InitializeComponent();
            splashScreen.Source = new BitmapImage(new Uri(splashScreenName));
            var timer = new DispatcherTimer { Interval = SplashDuration };
            timer.Tick += (_, _) =>
            {
                timer.Stop();
                splashScreen.Source = new BitmapImage(new Uri(splashScreenBlank));
            };
            timer.Start();
        }

        protected override void OnInitialized(EventArgs args)
        {
            base.OnInitialized(args);

        }



    }
}
