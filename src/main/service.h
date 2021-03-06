/*
 *  Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd
 *
 * Author:  wangpeili<wangpeili@uniontech.com>
 *
 * Maintainer:wangpeili<wangpeili@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SERVICE_H
#define SERVICE_H

#include "termproperties.h"
#include "windowsmanager.h"
#include "shortcutmanager.h"
#include "settings.h"
#include "atspidesktop.h"

#include <DSettingsDialog>
#include <DDialog>
#include <QObject>
#include <QSharedMemory>

#include <com_deepin_wmswitcher.h>

using WMSwitcher = com::deepin::WMSwitcher;
#define WMSwitcherService "com.deepin.WMSwitcher"
#define WMSwitcherPath "/com/deepin/WMSwitcher"


DWIDGET_USE_NAMESPACE
struct ShareMemoryInfo {
    int enableCreateTerminal = 0;
    int TerminalsCount = 0;
    qint64 appStartTime = 0; //sub app 启动的时间
};
/*******************************************************************************
 1. @类名:    Service
 2. @作者:    ut000439 王培利
 3. @日期:    2020-05-19
 4. @说明:    全局的后台服务类．
　　　　　　　　管理所有底层数据的加载
　　　　　　　　管理所有特殊的通知信号
　　　　　　　　管理所有程序入口
*******************************************************************************/
class Service : public QObject
{
    Q_OBJECT
public:
    static Service *instance();
    ~Service();
    void init();
    // 初始化设置框
    void initSetting();

    // 显示设置框
    void showSettingDialog(MainWindow *pOwner);
    // 显示设置快捷键冲突弹窗
    void showShortcutConflictMsgbox(QString txt);
    // 设置框是否显示
    bool isSettingDialogVisible()
    {
        return m_settingDialog ? m_settingDialog->isVisible() : false;
    }
    //获取设置框的所有者
    MainWindow *getSettingOwner()
    {
        return m_settingOwner;
    }
    //重置设置框的所有者
    void resetSettingOwner()
    {
        m_settingOwner = nullptr;
    }

    // 是否允许继续创建
    bool isCountEnable();

    bool getIsDialogShow() const;
    void setIsDialogShow(QWidget *parent, bool isDialogShow);

    // 主进程是否允许创建新的窗口，如果允许，把子进程的创建时间设置到共享内存传递过去。
    bool getEnable(qint64 time) ;
    // 设置子进程启动时间到共享内存
    void setSubAppStartTime(qint64);
    // 获取子进程在共享的启动时间
    qint64 getSubAppStartTime();

    void updateShareMemoryCount(int count);
    int  getShareMemoryCount();
    bool setMemoryEnable(bool enable);
    bool getMemoryEnable();
    void releaseShareMemory();

    //判断当前是否开启窗口特效  开启-true 关闭-false
    bool isWindowEffectEnabled();
    // 获取主程序初始进入的时间
    qint64 getEntryTime();

signals:
    void refreshCommandPanel(QString oldCmdName, QString newCmdName);
    // 切换编码列表的编码
    void checkEncode(QString encode);
    // 当前终端切换
    void currentTermChange(QWidget *term);

    void onWindowEffectEnabled(bool isEnabled);

public slots:
    void Entry(QStringList arguments);
private:
    explicit Service(QObject *parent = nullptr);
    //显示/隐藏设置透明度和背景模糊选项-- 仅UOS服务器版本使用
    void showHideOpacityAndBlurOptions(bool isShow);
    //监听窗口特效开关对应DBus信号，并实时显示/隐藏透明度和背景模糊选项
    void listenWindowEffectSwitcher();

    static Service *pService ;
    // 设置框 全局唯一显示
    DSettingsDialog *m_settingDialog = nullptr;

    WMSwitcher *m_wmSwitcher = nullptr;
    //设置框的所有者
    MainWindow *m_settingOwner = nullptr;
    // 设置框，快捷键冲突弹窗
    DDialog *m_settingShortcutConflictDialog = nullptr;
    // 雷神用来判断是否有弹窗显示
    bool m_isDialogShow = false;
    // 是否允许创建新的窗口
    bool m_enable = false;
    QSharedMemory *m_enableShareMemory = nullptr;
    // 这个指针实际上与上面指针指向同一地址，不需要二次释放
    ShareMemoryInfo *m_pShareMemoryInfo = nullptr;
    // 初始化和运行无障碍辅助工具的线程
    AtspiDesktop *m_atspiThread = nullptr;

    // 记录进入的时间，只有创建窗口时，才会来取用这个时间
    qint64 m_EntryTime = 0;
};

#endif // SERVICE_H
