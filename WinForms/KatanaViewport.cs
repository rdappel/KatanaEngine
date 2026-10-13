using System.ComponentModel;
using System.Runtime.InteropServices;

namespace KatanaEngine.WinForms;

/// <summary>
/// Hosts a KatanaEngine game inside a WinForms layout. The game's window becomes a
/// borderless child of this control and runs on its own thread.
///
/// Set <see cref="GameLibraryPath"/> to a game DLL built with KATANA_DEFINE_HOST_EXPORTS.
/// The game starts when the control's handle is created and stops when it is destroyed.
/// Talk to it with <see cref="Post"/>; replies arrive through <see cref="MessageReceived"/>
/// on the UI thread.
/// </summary>
public class KatanaViewport : Control
{
    private NativeGameLibrary? _library;
    private NativeGameLibrary.MessageCallback? _messageCallback;  // kept alive while native code holds it
    private string? _gameLibraryPath;
    private string? _startError;

    public KatanaViewport()
    {
        SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.Opaque, true);
        BackColor = System.Drawing.Color.FromArgb(31, 31, 36);
    }

    /// <summary>Raised on the UI thread for each message the game sends.</summary>
    public event EventHandler<string>? MessageReceived;

    /// <summary>Path to the game DLL; relative paths resolve against the app's base directory.</summary>
    [DefaultValue(null)]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
    public string? GameLibraryPath
    {
        get => _gameLibraryPath;
        set
        {
            if (IsRunning) throw new InvalidOperationException("Stop the game before changing GameLibraryPath.");

            _gameLibraryPath = value;
            if (IsHandleCreated) StartGame();
        }
    }

    [Browsable(false)]
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    public bool IsRunning { get; private set; }

    /// <summary>Sends a message to the game. Ignored while the game is not running.</summary>
    public void Post(string message)
    {
        if (IsRunning) _library?.Post(message);
    }

    protected override void OnHandleCreated(EventArgs e)
    {
        base.OnHandleCreated(e);
        StartGame();
    }

    protected override void OnHandleDestroyed(EventArgs e)
    {
        // Stop while this window still exists: the game's window is our child.
        StopGame();
        base.OnHandleDestroyed(e);
    }

    protected override void OnResize(EventArgs e)
    {
        base.OnResize(e);
        if (IsRunning && ClientSize.Width > 0 && ClientSize.Height > 0)
            _library?.Resize(ClientSize.Width, ClientSize.Height);
    }

    protected override void OnPaint(PaintEventArgs e)
    {
        // Only visible before the game starts (or if it failed); the game's window covers it otherwise.
        e.Graphics.Clear(BackColor);

        var text = _startError ?? (DesignMode ? "KatanaViewport" : "Starting...");
        TextRenderer.DrawText(e.Graphics, text, Font, ClientRectangle, System.Drawing.Color.Gainsboro,
            TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter | TextFormatFlags.WordBreak);
    }

    private void StartGame()
    {
        if (IsRunning || DesignMode || string.IsNullOrWhiteSpace(_gameLibraryPath)) return;

        try
        {
            _library ??= NativeGameLibrary.Load(_gameLibraryPath);
            _messageCallback ??= OnNativeMessage;
            IsRunning = _library.Start(Handle, _messageCallback);
            _startError = IsRunning ? null : "The game failed to start.";
        }
        catch (Exception ex) when (ex is DllNotFoundException or EntryPointNotFoundException or BadImageFormatException)
        {
            _startError = $"Could not load {_gameLibraryPath}: {ex.Message}";
        }

        Invalidate();
    }

    private void StopGame()
    {
        if (!IsRunning) return;

        IsRunning = false;
        _library?.Stop();
    }

    // Called on the game thread.
    private void OnNativeMessage(IntPtr utf8Message)
    {
        var message = Marshal.PtrToStringUTF8(utf8Message);
        if (message == null || !IsHandleCreated) return;

        try
        {
            BeginInvoke(() => MessageReceived?.Invoke(this, message));
        }
        catch (InvalidOperationException)
        {
            // The handle is being destroyed; the game is shutting down.
        }
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing) StopGame();
        base.Dispose(disposing);
    }
}
