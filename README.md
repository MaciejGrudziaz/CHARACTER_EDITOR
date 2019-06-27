# CHARACTER EDITOR
Character editor built for [**Project Mayhem**](https://github.com/MaciejGrudziaz/PROJECT_MAYHEM.git). It can only read **.fbx** files,
which can be created using external programs [like Blender], and **.mgr** files. After reading the file, it automatically creates 
approximate hitboxes for all joints found in skeleton, to best fit the joint and displays effects on the screen.

User can control the editor using special text console interface and graphical interface for displaying the effects.
The main goal of this editor is to check if the model and the file was properly created in external program, if all the animations
are working and to create necessary hitboxes for the model.
User can scale and move automatically created hitboxes, so they can better fit the model and if the model is ready and checked, 
user can export it **.mgr** file so it can be read by the **Project Mayhem** API.

## GALLERY

Character editor preview window and console interface
![character editor screenshot](https://github.com/MaciejGrudziaz/CHARACTER_EDITOR/blob/master/Screenshots/editor_screenshot_1.png)

Loading 3D model to editor and automatically calculating hitboxes for every joint (red boxes).
![character editor screenshot](https://github.com/MaciejGrudziaz/CHARACTER_EDITOR/blob/master/Screenshots/editor_screenshot_2.png)

Animation preview.
![character editor screenshot](https://github.com/MaciejGrudziaz/CHARACTER_EDITOR/blob/master/Screenshots/editor_screenshot_3.png)

### Editing hitboxes
Transforming basic hitboxes (the green box is currently chosen hitbox for transformation).
![character editor screenshot](https://github.com/MaciejGrudziaz/CHARACTER_EDITOR/blob/master/Screenshots/editor_screenshot_4.png)

Linkig multiple hitboxes together (left arm hitboxes are linked to create one hitbox).
![character editor screenshot](https://github.com/MaciejGrudziaz/CHARACTER_EDITOR/blob/master/Screenshots/editor_screenshot_5.png)
