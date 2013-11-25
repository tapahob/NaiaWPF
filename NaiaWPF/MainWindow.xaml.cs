using System;
using System.Windows;
using NaiaLib;

namespace NaiaWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            this.Loaded += MainWindow_Loaded;
        }



        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            NaiaLib.EditorCore.Instance.Initialize();
            NaiaLib.SceneWrapper.Init();

            WinFormsHost1.Child = new OpenGLControl();
            WinFormsHost2.Child = new OpenGLControl();
            WinFormsHost3.Child = new OpenGLControl();
            WinFormsHost4.Child = new OpenGLControl();

            NaiaLib.EditorCore.Instance.StartMainLoop();
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            NaiaLib.SceneWrapper.AddMeshNode();
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            NaiaLib.EditorCore.Instance.Shutdown();
        }
    }
}
