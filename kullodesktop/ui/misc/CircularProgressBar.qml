/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

ProgressBar {
    /* public */
    property color backgroundColor: "transparent"
    property color color: "#164291"
    property real radius1: 1.0
    property real radius2: 0.5
    property real speed: 1 // rounds per seconds

    /* private */
    id: root
    property int _defaultWidth: 32
    property int _defaultHeight: 32

    Component {
        id: circularProgressBarStyle

        ProgressBarStyle {
            background: Rectangle {
                color: root.backgroundColor
                implicitWidth: root._defaultWidth
                implicitHeight: root._defaultHeight
            }

            progress: Item {
                width: root.width
                height: root.height

                function ratioToArgument(x) {
                    return x*2*Math.PI - 0.5*Math.PI
                }

                function makeCirclePart(ctx, from, to, color) {
                    var centerX = root.width/2
                    var centerY = root.height/2
                    var radius1Px = root.radius1 * root.width/2
                    var radius2Px = root.radius2 * root.width/2

                    ctx.beginPath();
                    ctx.fillStyle = color;

                    ctx.moveTo(centerX, centerY);
                    ctx.arc(centerX, centerY, radius1Px,
                            ratioToArgument(from), ratioToArgument(to),
                            false);

                    var startArc2X = centerX + Math.sin(to * 2*Math.PI)*radius2Px
                    var startArc2Y = centerY - Math.cos(to * 2*Math.PI)*radius2Px
                    ctx.lineTo(startArc2X, startArc2Y);

                    ctx.arc(centerX, centerY, radius2Px,
                            ratioToArgument(to), ratioToArgument(from),
                            true);
                    ctx.fill()
                    //ctx.stroke()
                }

                Item {
                    // force override parent's width, independent of progress
                    width: root.width
                    height: root.height

                    Connections {
                        target: root

                        function update() {
                            if (!control.indeterminate) {
                                canvasDeterminate.requestPaint()
                            }
                        }

                        onValueChanged: update()
                        onMinimumValueChanged: update()
                        onMaximumValueChanged: update()
                    }

                    Canvas {
                        id: canvasIndeterminate
                        anchors.fill: parent
                        visible: control.indeterminate

                        onPaint: {
                            var context = getContext("2d");
                            context.reset();

                            var begin = 0
                            var end = 0.02

                            var color = root.color

                            // color_points=50 steps=49 max=100 min=2 => range=98
                            for (var count = 0; count <= 49; ++count)
                            {
                                var step = count*0.02
                                makeCirclePart(context,
                                               begin-step, end-step,
                                               Qt.rgba(color.r, color.g, color.b, 1.00-step))
                            }
                        }

                        RotationAnimation on rotation {
                            loops: Animation.Infinite
                            from: 0
                            to: 360
                            duration: 1000 / speed
                            direction: RotationAnimation.Clockwise
                        }
                    }

                    Canvas {
                        id: canvasDeterminate
                        anchors.fill: parent
                        visible: !control.indeterminate

                        onPaint: {
                            var context = getContext("2d");
                            context.reset();
                            var begin = 0
                            var end = (value-minimumValue) / (maximumValue-minimumValue)

                            if (end > 0.99) {
                                // woraround for fill bug when circle is closed
                                makeCirclePart(context, begin, 0.5, root.color)
                                makeCirclePart(context, 0.5, end, root.color)
                            } else {
                                makeCirclePart(context, begin, end, root.color)
                            }
                        }
                    }
                }
            }
        }
    }

    style: circularProgressBarStyle
}
