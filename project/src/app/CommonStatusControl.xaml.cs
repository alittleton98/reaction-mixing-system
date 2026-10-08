using System.Windows;
using System.Windows.Controls;

namespace ReactionMixingSystem;

/// <summary>
/// Interaction logic for CommonStatusControl.xaml
/// </summary>
public partial class CommonStatusControl : UserControl
{
    public event RoutedEventHandler? InitializeRequested;

    public CommonStatusControl()
    {
        InitializeComponent();
    }

    public void SetStatus(string text)
    {
        StatusText.Text = text;
    }

    private void InitializeButton_Click(object sender, RoutedEventArgs e)
    {
        InitializeRequested?.Invoke(this, e);
    }
}
