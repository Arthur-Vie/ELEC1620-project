import bpy

import sys
sys.path.append("/home/arthur/.local/lib/python3.7/site-packages")

import serial
import math

serial_port = serial.Serial('/dev/ttyUSB0', 19200)
    
def convert_to_quaternion(z, y, x):
    cz = math.cos(z * 0.5)
    sz = math.sin(z * 0.5)
    cy = math.cos(y * 0.5)
    sy = math.sin(y * 0.5)
    cx = math.cos(x * 0.5)
    sx = math.sin(x * 0.5)
    
    quaternion = [0, 0, 0, 0]
    quaternion[0] = cz * cy * cx + sz * sy * sx
    quaternion[1] = sz * cy * cx - cz * sy * sx
    quaternion[2] = cz * sy * cx + sz * cy * sx
    quaternion[3] = cz * cy * sx - sz * sy * cx
    
    return quaternion

#bpy.data.scenes["Scene"].transform_orientation_slots[0].type = 'NORMAL'
bpy.data.objects["Cube"].rotation_mode = 'QUATERNION' 

class ModalTimerOperator(bpy.types.Operator):
    """Operator which runs its self from a timer"""
    bl_idname = "wm.modal_timer_operator"
    bl_label = "Modal Timer Operator"

    _timer = None

    def modal(self, context, event):
        if event.type in {'RIGHTMOUSE', 'ESC'}:
            self.cancel(context)
            serial_port.close()
            return {'CANCELLED'}

        if event.type == 'TIMER':
            
            negative = False
            numeric_values = [0, 0, 0, 0, 0, 0]

            index = 0
            data_line = serial_port.readline()#serial_port.inWaiting)
            data_line = data_line[1:]
            text_data_values = data_line.split(b",\t", 6)
            for element in text_data_values:
        
                if element[0] == 45:
                    negative = True
                else:
                    negative = False
            
                element = element[1:]
        
                if element.isdigit():
                    numeric_values[index] = int(element)
        
                    if negative:
                        numeric_values[index] = numeric_values[index] * -1
                
        
                index = index + 1
            
            rotation_radians = [0, 0, 0]
            rotation_radians[0] = numeric_values[0] * (math.pi / 180)
            rotation_radians[1] = (360 - numeric_values[2]) * (math.pi / 180)
            rotation_radians[2] = numeric_values[4] * (math.pi / 180)
            bpy.data.objects["Cube"].rotation_quaternion = convert_to_quaternion(rotation_radians[0], rotation_radians[1], rotation_radians[2])

        return {'PASS_THROUGH'}

    def execute(self, context):
        wm = context.window_manager
        self._timer = wm.event_timer_add(0.1, window=context.window)
        wm.modal_handler_add(self)
        return {'RUNNING_MODAL'}

    def cancel(self, context):
        wm = context.window_manager
        wm.event_timer_remove(self._timer)


def register():
    bpy.utils.register_class(ModalTimerOperator)


def unregister():
    bpy.utils.unregister_class(ModalTimerOperator)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.wm.modal_timer_operator()
