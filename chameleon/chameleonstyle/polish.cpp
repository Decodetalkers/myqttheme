//#include "chameleonstyle.h"
//#include <DPlatformWindowHandle>
//#include <DWindowManagerHelper>
//#include <dtabbar.h>
//#include <DGuiApplicationHelper>
//#include <DIconButton>
//#include <QCheckBox>
//#include <dfontmanager.h>
//#include <private/qcombobox_p.h>

#include "chameleonstyle.h"
#include "chameleontools.hpp"
#include <DApplication>
#include <DButtonBox>
#include <DNativeSettings>
#include <DPlatformTheme>
#include <DPlatformWindowHandle>
#include <DSearchEdit>
#include <DWindowManagerHelper>

#include <DDateTimeEdit>
#include <DListView>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QScrollBar>
#include <QTableView>
#include <private/qcombobox_p.h>

#include <qpa/qplatformwindow.h>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

namespace chameleon {

void ChameleonStyle::polish(QWidget *w)
{
    DStyle::polish(w);

    resetAttribute(w, true);

    if (qobject_cast<QAbstractButton *>(w) || qobject_cast<QComboBox *>(w) || qobject_cast<QScrollBar *>(w) ||
        qobject_cast<QCheckBox *>(w) || qobject_cast<QAbstractSpinBox *>(w) || qobject_cast<QTabBar *>(w) ||
        qobject_cast<QCheckBox *>(w)) {
        w->setAttribute(Qt::WA_Hover, true);
    }

    if (auto view = qobject_cast<QAbstractItemView *>(w)) {
        view->viewport()->setAttribute(Qt::WA_Hover, true);
    }

    if (auto listview = qobject_cast<QListView *>(w)) {
        if (listview->parentWidget() == nullptr) {
            DPlatformWindowHandle handle(listview);
            handle.setWindowRadius(DStyle::pixelMetric(PM_FrameRadius));
        }
    }

    if (w && qobject_cast<QLineEdit *>(w)) {
        w->setProperty("_d_dtk_lineeditActionWidth", -6);
        w->setProperty("_d_dtk_lineeditActionMargin", 6);
    }

    if (auto container = qobject_cast<QComboBoxPrivateContainer *>(w)) {
        if (DWindowManagerHelper::instance()->hasComposite()) {
            DPlatformWindowHandle handle(container);
            handle.setWindowRadius(DStyle::pixelMetric(PM_FrameRadius));
        }
        if (!DGuiApplicationHelper::isTabletEnvironment())
            container->setFrameStyle(QFrame::NoFrame);
    }

    if (auto calendar = qobject_cast<QCalendarWidget *>(w)) {
        int radius = DStyle::pixelMetric(PM_TopLevelWindowRadius);
        // ??????dtk?????????????????????????????????
        if (dynamic_cast<DApplication *>(QCoreApplication::instance())) {
            DPlatformWindowHandle handle(calendar);
            handle.setWindowRadius(radius);
        }

        calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

        // ????????????Saturday???Sunday???????????????????????????
        DPlatformTheme *theme = DGuiApplicationHelper::instance()->applicationTheme();
        updateWeekendTextFormat(calendar, QColor());
        connect(theme,
                &DPlatformTheme::activeColorChanged,
                calendar,
                std::bind(&updateWeekendTextFormat, calendar, std::placeholders::_1));

        auto topWidget = calendar->findChild<QWidget *>("qt_calendar_navigationbar");
        topWidget->setBackgroundRole(QPalette::Base);

        auto layout = qobject_cast<QLayout *>(topWidget->layout());
        layout->setMargin(radius / 2);
    }

    if (w && (w->objectName() == "qt_calendar_yearbutton" || w->objectName() == "qt_calendar_monthbutton")) {
        w->setProperty("_d_calendarToolBtn", true);

        DFontSizeManager *fontManager = DFontSizeManager::instance();
        fontManager->bind(w, DFontSizeManager::T5, QFont::Normal);
    }

    if (w && w->objectName() == "qt_calendar_yearedit") {
        w->setProperty("_d_dtk_spinBox", true);
        //????????????spinBox????????????
        int width = w->fontMetrics().horizontalAdvance("9999");
        w->setMaximumWidth(width * 3);
    }

    if (w && (w->objectName() == "qt_calendar_prevmonth" || w->objectName() == "qt_calendar_nextmonth")) {
        int btnWidget = DStyle::pixelMetric(DStyle::PM_ButtonMinimizedSize);
        w->setMinimumSize(btnWidget, btnWidget);
    }

    if (w && w->objectName() == "qt_calendar_calendarview") {
        auto view = qobject_cast<QTableView *>(w);
        view->setItemDelegate(new QStyledItemDelegate);
    }

    if (DApplication::isDXcbPlatform() || (qApp->platformName() == "dwayland" || qApp->property("_d_isDwayland").toBool())) {
        bool is_menu = qobject_cast<QMenu *>(w);
        bool is_tip = w->inherits("QTipLabel");

        // ??????????????????????????????native???????????????????????????????????????????????????????????????
        // Bug: https://github.com/linuxdeepin/internal-discussion/issues/323
        if (is_menu && w->windowHandle()) {
            if (const QPlatformWindow *handle = w->windowHandle()->handle()) {
                if (!w->testAttribute(Qt::WA_TranslucentBackground) && !handle->isExposed()) {
                    // ???????????????native?????????????????????Qt::WA_TranslucentBackground????????????
                    class DQWidget : public QWidget
                    {
                    public:
                        using QWidget::destroy;
                    };
                    reinterpret_cast<DQWidget *>(w)->destroy(true, false);
                }
            }
        }

        // fix qtcreator QDesignerMenu setnotitlebar qmainwindow
        if (is_menu && w->isWindow()) {
            DPlatformWindowHandle handle(w);

            if (DPlatformWindowHandle::isEnabledDXcb(w)) {
                handle.setEnableBlurWindow(true);
                // ????????????8, 18???????????????????????????8
                auto theme = DGuiApplicationHelper::instance()->applicationTheme();
                int wradius = theme->windowRadius();
                handle.setWindowRadius(qMax(0, qMin(wradius, 8)));
                w->setAttribute(Qt::WA_TranslucentBackground);

                connect(DWindowManagerHelper::instance(), SIGNAL(hasCompositeChanged()), w, SLOT(update()));
            }
        } else if (is_tip) {
            if (DWindowManagerHelper::instance()->hasComposite()) {
                DPlatformWindowHandle handle(w);
                handle.setWindowRadius(DStyle::pixelMetric(PM_FrameRadius));
            }
            QLabel *label = qobject_cast<QLabel *>(w);
            label->setTextFormat(DStyle::tooltipTextFormat());
        }
    }
}
}  // namespace chameleon
