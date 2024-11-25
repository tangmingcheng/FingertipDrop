import QtQuick

StartScrennForm {
    id:startscreen

    property int minDuration: 300
    property int maxDuration: 3000
    rotationAnimSpeed:2000

    function calculateDuration(mouseX, mouseY) {
        let buttonCenterX = startbuttonWidth / 2;
        let buttonCenterY = startbuttonHeight / 2;
        let dx = mouseX - buttonCenterX;
        let dy = mouseY - buttonCenterY;
        let distance = Math.sqrt(dx * dx + dy * dy);
        // 使用指数公式计算 speedFactor
        let a = 5; // 当 distance = 0 时 speedFactor
        let b = Math.pow(1 / 5, 1 / 150); // 指数衰减因子
        let speedFactor = a * Math.pow(b, distance);

        return 2000 / speedFactor;
    }





     onPositionChanged: (x,y)=>{
                            rotationAnimSpeed = calculateDuration(x,y)
                        }

}
