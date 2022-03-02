// This is a header file serving only to hold QSS code.
const char* DARK_QSS = R"(

QDialog {
    background: #333;
    border: none;
    color: #ddd;
}


QDockWidget {
    background: #333;
    border: none;
    color: #ddd;
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
}

QDockWidget::title {
    background: #444;
    border: none;
    color: #ddd;
    padding: 6px;
    text-align: left;
}

QDockWidget::close-button, QDockWidget::float-button {
    border-radius: 6px;
    max-height: 8px;
    max-width: 8px;
    min-height: 8px;
    min-width: 8px;
    position: relative;
    subcontrol-origin: padding;
    subcontrol-position: right center;
}

QDockWidget::close-button {
    background: transparent;
    border: 2px solid #e84b33;
    right: 4px;
}

QDockWidget::float-button {
    background: transparent;
    border: 2px solid #888;
    right: 24px;
}

QDockWidget::close-button:hover, QDockWidget::close-button:pressed {
    background: #e84b33;
}

QDockWidget::float-button:hover, QDockWidget::float-button:pressed {
    background: #888;
}


QListWidget {
    background: #222;
    border: none;
    border-radius: 8px;
    color: #ddd;
    margin: 8px;
    padding: 8px;
}


QMainWindow {
    background: #333;
    border: none;
    color: #ddd;
}

QMainWindow::separator {
    background: #555;
    border: none;
    /*border-right: 2px solid #666;*/
    width: 8px;
    height: 2px;
}

QMainWindow::separator:hover {
    /*border-right-color: #39a0fa;*/
}


QToolBar {
    background: #444;
    border: none;
    border-bottom: 2px solid #666;
    color: #ddd;
    padding: 2px;
}

QToolBar::separator {
    border-left: 2px solid #666;
    margin: 6px 2px;
    width: 1px;
}


QToolButton {
    background: #444;
    border: none;
    border-radius: 4px;
    color: #ddd;
    margin: 2px 2px 3px 2px;
    padding: 4px;
}

QToolButton:hover {
    background: #555;
}

QToolButton:pressed {
    background: #333;
}

QToolButton:disabled {
    background: #555;
    color: #ddd;
}

QToolButton:checked {
    background: #333;
}


QToolTip {
    background: #444;
    border: none;
    border-radius: 3px;
    color: #ddd;
    padding: 2px 3px;
}

)";