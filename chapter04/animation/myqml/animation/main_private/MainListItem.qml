﻿/*ListView 的代理*/
import QtQuick 2.11
import QtQuick.Controls 1.4

Button {

    height: 32 ;
    text : programName ;
    tooltip: qsTr("案例所在文件：") + programSource ;

    function loadSourceFunction(){
        var loader = Qt.createComponent(programSource);
         if (loader.status === Component.Ready) {
             var obj = loader.createObject();
             obj.show()/* window will be destoryed when close */;
             engine.clearAllComponentCache()/*支持热更新*/;
         }
         loader.destroy();
    }/*loadSourceFunction*/

    onClicked : {
        loadSourceFunction();
    }

}/*Button*/




