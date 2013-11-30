using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
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
            IntPtr windowHandle = new WindowInteropHelper(this).EnsureHandle();
            IntPtr appHandle = Marshal.GetHINSTANCE(typeof(App).Module);

            NaiaLib.EditorCore.Instance.Initialize(appHandle, windowHandle);
            NaiaLib.SceneWrapper.Init();

            System.Windows.Controls.TreeViewItem a = SceneTreeRoot;

            WinFormsHost1.Child = new OpenGLControl();
            WinFormsHost2.Child = new OpenGLControl();
            WinFormsHost3.Child = new OpenGLControl();
            WinFormsHost4.Child = new OpenGLControl();

            NaiaLib.EditorCore.Instance.StartMainLoop();
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            NaiaLib.SceneWrapper.AddMeshNode();
            NaiaLib.SceneWrapper.UpdateSceneWindow(SceneTreeRoot);
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            NaiaLib.EditorCore.Instance.Shutdown();
        }
    }
}
