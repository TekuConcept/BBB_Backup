# ##### BEGIN GPL LICENSE BLOCK #####
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software Foundation,
#  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# ##### END GPL LICENSE BLOCK #####

# <pep8 compliant>

# Changes in Version 1.0.3:
# - Remapped heavilly used menus to mouse buttons on a 5-button mouse
# - Added 'Create Mesh' menu for quick convinient access
# - Added 'Selection' menu for deselection in world space,
#     rapidly toggling between edit-mode and object-mode,
#     and quickly selecting the type of edit mode (vertex, edge, face)
# - Added 'Snap|Origin' for quick 3d locator and object origin in absence
#     of Maya's Insert hotkey
# - Updated Camera Pie Menu so that its all in one place
#     (seems slightly slower IMO, but not enough to slow development)
# - Add hotkey with Maya-like origin manipulation aka 'Insert'

bl_info = {
    "name": "Pie Menus Unofficial",
    "author": "Antony Riakiotakis, Sebastian Koenig, Chris Walker",
    "version": (1, 0, 3),
    "blender": (2, 77, 1),
    "description": "Enable official Pie Menus in Blender",
    "category": "User Interface",
    "wiki_url": "http://wiki.blender.org/index.php/Dev:Ref/Release_Notes/2.72/UI/Pie_Menus"
}


import bpy
from bpy.types import Menu, Operator
from bpy.props import EnumProperty


class PIE_origin_helper(bpy.types.Operator):
    bl_idname = "view3d.set_selected_origin"
    bl_label = "Set Selected Origin"

    def execute(self, context):
        bpy.ops.view3d.snap_cursor_to_selected()
        _mode_ = context.active_object.mode
        if _mode_ == "OBJECT":
            bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
        else:
            bpy.ops.object.mode_set(mode="OBJECT")
            bpy.ops.object.origin_set(type="ORIGIN_CURSOR")
            bpy.ops.object.mode_set(mode=_mode_)
        return {'FINISHED'}


class VIEW3D_PIE_snap_origin(Menu):
    bl_label = "Snap | Origin"

    def draw(self, context):
        layout = self.layout
        pie = layout.menu_pie()

        group = pie.column()
        box = group.box()
        box.operator("view3d.snap_selected_to_grid", text="Selection to Grid")
        box.operator("view3d.snap_selected_to_cursor", text="Selection to Cursor").use_offset = False
        box.operator("view3d.snap_selected_to_cursor", text="Selection to Cursor (Offset)").use_offset = True
        box.separator()
        box.operator("view3d.snap_cursor_to_selected", text="Cursor to Selected")
        box.operator("view3d.snap_cursor_to_center", text="Cursor to Center")
        box.operator("view3d.snap_cursor_to_grid", text="Cursor to Grid")
        box.operator("view3d.snap_cursor_to_active", text="Cursor to Active")

        group2 = pie.column()
        box2 = group2.box()
        box2.operator("object.origin_set", text="Geometry to Origin").type = 'GEOMETRY_ORIGIN'
        box2.operator("object.origin_set", text="Origin to Geometry").type = 'ORIGIN_GEOMETRY'
        box2.operator("object.origin_set", text="Origin to 3D Cursor").type = 'ORIGIN_CURSOR'
        box2.operator("object.origin_set", text="Origin to Center of Mass").type = 'ORIGIN_CENTER_OF_MASS'
        box2.operator("view3d.set_selected_origin", text="Origin to Selection")



class VIEW3D_PIE_select_mode(Menu):
    bl_label = "Edit Mode"

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        mode = context.active_object.mode
        if(mode == 'OBJECT'):
            pie.operator("object.select_all", text="Deselect").action = 'DESELECT'
            pie.operator("object.mode_set", text="Edit Mode", icon="EDITMODE_HLT").mode = 'EDIT'
        elif(mode == 'EDIT'):
            pie.operator("mesh.select_mode", text="Vertex", icon='VERTEXSEL').type = 'VERT'
            pie.operator("mesh.select_all", text="Deselect").action = 'DESELECT'
            pie.operator("mesh.select_mode", text="Face", icon="FACESEL").type = 'FACE'
            pie.operator("mesh.select_mode", text="Edge", icon="EDGESEL").type = 'EDGE'
            pie.operator("object.mode_set", text="Object Mode", icon="OBJECT_DATAMODE").mode = 'OBJECT'
            pie.operator("wm.call_menu_pie", text="Manipulator", icon="MANIPUL").name = "VIEW3D_PIE_manipulator"


class VIEW3D_PIE_create(Menu):
    bl_label = "Create Mesh"

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        pie.operator("mesh.primitive_plane_add", text="Plane", icon='MESH_PLANE')
        pie.operator("mesh.primitive_cube_add", text="Cube", icon='MESH_CUBE')
        pie.operator("mesh.primitive_circle_add", text="Circle", icon='MESH_CIRCLE')
        pie.operator("mesh.primitive_uv_sphere_add", text="UV Sphere", icon='MESH_UVSPHERE')
        #pie.operator("mesh.primitive_ico_sphere_add", text="Ico Sphere", icon='MESH_ICOSPHERE')
        pie.operator("mesh.primitive_cylinder_add", text="Cylinder", icon='MESH_CYLINDER')
        pie.operator("mesh.primitive_cone_add", text="Cone", icon='MESH_CONE')
        pie.operator("mesh.primitive_torus_add", text="Torus", icon='MESH_TORUS')
        #pie.operator("mesh.primitive_grid_add", text="Grid", icon='MESH_GRID')
        pie.operator("mesh.primitive_monkey_add", text="Monkey", icon='MESH_MONKEY')


class VIEW3D_PIE_object_mode(Menu):
    bl_label = "Mode"

    def create():
        VIEW3D_PIE_create(Menu)

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        pie.operator_enum("OBJECT_OT_mode_set", "mode")
        pie.operator("wm.call_menu_pie", text="Create Mesh", icon='PLUS').name = "VIEW3D_PIE_create"


#class VIEW3D_PIE_view_more(Menu):
#    bl_label = "More"

#    def draw(self, context):
#        layout = self.layout

#        pie = layout.menu_pie()
#        pie.operator("VIEW3D_OT_view_persportho", text="Persp/Ortho", icon='RESTRICT_VIEW_OFF')
#        pie.operator("VIEW3D_OT_camera_to_view")
#        pie.operator("VIEW3D_OT_view_selected")
#        pie.operator("VIEW3D_OT_view_all")
#        pie.operator("VIEW3D_OT_localview")
#        pie.operator("SCREEN_OT_region_quadview")


class PIE_screen_helper(bpy.types.Operator):
    bl_idname = "cam.screen_helper"
    bl_label = "Camera Screen Helper"

    def execute(self, context):
        if context.screen.name != 'Default':
            context.window.screen = bpy.data.screens["Default"]
        else:
            context.window.screen = bpy.data.screens["3D View Full"]
        return {'FINISHED'}


class VIEW3D_PIE_view(Menu):
    bl_label = "View"

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        pie.operator("view3d.viewnumpad", text="Left").type = 'LEFT'
        pie.operator("view3d.viewnumpad", text="Right").type = 'RIGHT'
        pie.operator("view3d.viewnumpad", text="Bottom").type = 'BOTTOM'
        pie.operator("view3d.viewnumpad", text="Top").type = 'TOP'
        pie.operator("view3d.viewnumpad", text="Front").type = 'FRONT'
        pie.operator("view3d.viewnumpad", text="Back").type = 'BACK'
        
        box = pie.split().column()
        box.row(align=True).operator("VIEW3D_OT_view_persportho", text="Persp/Ortho", icon='RESTRICT_VIEW_OFF')
        box.row(align=True).operator("view3d.viewnumpad", text="Camera").type = 'CAMERA'
        box.row(align=True).operator("screen.region_quadview", text="Toggle Quad View")

        if context.screen.name == 'Default':
            box.row(align=True).operator("cam.screen_helper", text="Full Screen")
        else:
            box.row(align=True).operator("cam.screen_helper", text="Default")

        box = pie.split().column()
        box.row(align=True).operator("VIEW3D_OT_view_all")
        box.row(align=True).operator("VIEW3D_OT_view_selected")
        box.row(align=True).operator("VIEW3D_OT_localview")
        box.row(align=True).operator("VIEW3D_OT_camera_to_view")

        #pie.operator_enum("VIEW3D_OT_viewnumpad", "type")
        #pie.operator("wm.call_menu_pie", text="More", icon='PLUS').name = "VIEW3D_PIE_view_more"


class VIEW3D_PIE_shade(Menu):
    bl_label = "Shade"

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        pie.prop(context.space_data, "viewport_shade", expand=True)

        if context.active_object:
            if(context.mode == 'EDIT_MESH'):
                pie.operator("MESH_OT_faces_shade_smooth")
                pie.operator("MESH_OT_faces_shade_flat")
            else:
                pie.operator("OBJECT_OT_shade_smooth")
                pie.operator("OBJECT_OT_shade_flat")


class VIEW3D_manipulator_set(Operator):
    bl_label = "Set Manipulator"
    bl_idname = "view3d.manipulator_set"

    type = EnumProperty(
            name="Type",
            items=(('TRANSLATE', "Translate", "Use the manipulator for movement transformations"),
                   ('ROTATE', "Rotate", "Use the manipulator for rotation transformations"),
                   ('SCALE', "Scale", "Use the manipulator for scale transformations"),
                   ),
            )

    def execute(self, context):
        # show manipulator if user selects an option
        context.space_data.show_manipulator = True

        context.space_data.transform_manipulators = {self.type}

        return {'FINISHED'}


class VIEW3D_PIE_manipulator(Menu):
    bl_label = "Manipulator"

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        pie.operator("view3d.manipulator_set", icon='MAN_TRANS', text="Translate").type = 'TRANSLATE'
        pie.operator("view3d.manipulator_set", icon='MAN_ROT', text="Rotate").type = 'ROTATE'
        pie.operator("view3d.manipulator_set", icon='MAN_SCALE', text="Scale").type = 'SCALE'
        pie.prop(context.space_data, "show_manipulator")


class VIEW3D_PIE_pivot(Menu):
    bl_label = "Pivot"

    def draw(self, context):
        layout = self.layout

        pie = layout.menu_pie()
        pie.prop(context.space_data, "pivot_point", expand=True)
        if context.active_object.mode == 'OBJECT':
            pie.prop(context.space_data, "use_pivot_point_align", text="Center Points")


class VIEW3D_PIE_snap(Menu):
    bl_label = "Snapping"

    def draw(self, context):
        layout = self.layout

        toolsettings = context.tool_settings
        pie = layout.menu_pie()
        pie.prop(toolsettings, "snap_element", expand=True)
        pie.prop(toolsettings, "use_snap")


class CLIP_PIE_refine_pie(Menu):
    # Refinement Options
    bl_label = "Refine Intrinsics"

    def draw(self, context):
        clip = context.space_data.clip
        settings = clip.tracking.settings

        layout = self.layout
        pie = layout.menu_pie()

        pie.prop(settings, "refine_intrinsics", expand=True)


class CLIP_PIE_geometry_reconstruction(Menu):
    # Geometry Reconstruction
    bl_label = "Reconstruction"

    def draw(self, context):
        layout = self.layout
        pie = layout.menu_pie()

        pie.operator("clip.bundles_to_mesh", icon='MESH_DATA')
        pie.operator("clip.track_to_empty", icon='EMPTY_DATA')


class CLIP_PIE_proxy_pie(Menu):
    # Proxy Controls
    bl_label = "Proxy Size"

    def draw(self, context):
        space = context.space_data

        layout = self.layout
        pie = layout.menu_pie()

        pie.prop(space.clip, "use_proxy", text="Use Proxy")
        pie.prop(space.clip_user, "proxy_render_size", expand=True)


class CLIP_PIE_display_pie(Menu):
    # Display Options
    bl_label = "Marker Display"

    def draw(self, context):
        space = context.space_data

        layout = self.layout
        pie = layout.menu_pie()

        pie.prop(space, "show_names", text="Show Track Info", icon='WORDWRAP_ON')
        pie.prop(space, "show_disabled", text="Show Disabled Tracks", icon='VISIBLE_IPO_ON')
        pie.prop(space, "show_marker_search", text="Display Search Area", icon='VIEWZOOM')
        pie.prop(space, "show_marker_pattern", text="Display Pattern Area", icon='BORDERMOVE')


class CLIP_PIE_marker_pie(Menu):
    # Settings for the individual markers
    bl_label = "Marker Settings"

    def draw(self, context):
        clip = context.space_data.clip
        track_active = clip.tracking.tracks.active

        layout = self.layout
        pie = layout.menu_pie()

        prop = pie.operator("wm.context_set_enum", text="Loc", icon='OUTLINER_DATA_EMPTY')
        prop.data_path = "space_data.clip.tracking.tracks.active.motion_model"
        prop.value = "Loc"
        prop = pie.operator("wm.context_set_enum", text="Affine", icon='OUTLINER_DATA_LATTICE')
        prop.data_path = "space_data.clip.tracking.tracks.active.motion_model"
        prop.value = "Affine"

        pie.operator("clip.track_settings_to_track", icon='COPYDOWN')
        pie.operator("clip.track_settings_as_default", icon='SETTINGS')

        if track_active:
            pie.prop(track_active, "use_normalization", text="Normalization")
            pie.prop(track_active, "use_brute", text="Use Brute Force")
            pie.prop(track_active, "use_blue_channel", text="Blue Channel")

            if track_active.pattern_match == "PREV_FRAME":
                prop = pie.operator("wm.context_set_enum", text="Match Previous", icon='KEYINGSET')
                prop.data_path = "space_data.clip.tracking.tracks.active.pattern_match"
                prop.value = 'KEYFRAME'
            else:
                prop = pie.operator("wm.context_set_enum", text="Match Keyframe", icon='KEY_HLT')
                prop.data_path = "space_data.clip.tracking.tracks.active.pattern_match"
                prop.value = 'PREV_FRAME'


class CLIP_PIE_tracking_pie(Menu):
    # Tracking Operators
    bl_label = "Tracking"

    def draw(self, context):
        layout = self.layout
        pie = layout.menu_pie()

        prop = pie.operator("clip.track_markers", icon='PLAY_REVERSE')
        prop.backwards = True
        prop.sequence = True
        prop = pie.operator("clip.track_markers", icon='PLAY')
        prop.backwards = False
        prop.sequence = True

        pie.operator("clip.disable_markers", icon='RESTRICT_VIEW_ON')
        pie.operator("clip.detect_features", icon='ZOOM_SELECTED')

        pie.operator("clip.clear_track_path", icon='BACK').action = 'UPTO'
        pie.operator("clip.clear_track_path", icon='FORWARD').action = 'REMAINED'

        pie.operator("clip.refine_markers", icon='LOOP_BACK').backwards = True
        pie.operator("clip.refine_markers", icon='LOOP_FORWARDS').backwards = False


class CLIP_PIE_clipsetup_pie(Menu):
    # Setup the clip display options
    bl_label = "Clip and Display Setup"

    def draw(self, context):
        space = context.space_data

        layout = self.layout
        pie = layout.menu_pie()

        pie.operator("clip.reload", text="Reload Footage", icon='FILE_REFRESH')
        pie.operator("clip.prefetch", text="Prefetch Footage", icon='LOOP_FORWARDS')

        pie.prop(space, "use_mute_footage", text="Mute Footage", icon='MUTE_IPO_ON')
        pie.prop(space.clip_user, "use_render_undistorted", text="Render Undistorted")
        pie.operator("clip.set_scene_frames", text="Set Scene Frames", icon='SCENE_DATA')
        pie.operator("wm.call_menu_pie", text="Marker Display", icon='PLUS').name = "CLIP_PIE_display_pie"
        pie.operator("clip.set_active_clip", icon='CLIP')
        pie.operator("wm.call_menu_pie", text="Proxy", icon='PLUS').name = "CLIP_PIE_proxy_pie"


class CLIP_PIE_solver_pie(Menu):
    # Operators to solve the scene
    bl_label = "Solving"

    def draw(self, context):
        clip = context.space_data.clip
        settings = clip.tracking.settings

        layout = self.layout
        pie = layout.menu_pie()

        pie.operator("clip.create_plane_track", icon='MESH_PLANE')
        pie.operator("clip.solve_camera", text="Solve Camera", icon='OUTLINER_OB_CAMERA')

        pie.operator("wm.call_menu_pie", text="Refinement", icon='CAMERA_DATA').name = "CLIP_PIE_refine_pie"
        pie.prop(settings, "use_tripod_solver", text="Tripod Solver")

        pie.operator("clip.set_solver_keyframe", text="Set Keyframe A", icon='KEY_HLT').keyframe = 'KEYFRAME_A'
        pie.operator("clip.set_solver_keyframe", text="Set Keyframe B", icon='KEY_HLT').keyframe = 'KEYFRAME_B'

        prop = pie.operator("clip.clean_tracks", icon='STICKY_UVS_DISABLE')
        pie.operator("clip.filter_tracks", icon='FILTER')
        prop.frames = 15
        prop.error = 2


class CLIP_PIE_reconstruction_pie(Menu):
    # Scene Reconstruction
    bl_label = "Reconstruction"

    def draw(self, context):
        layout = self.layout
        pie = layout.menu_pie()

        pie.operator("clip.set_viewport_background", text="Set Viewport Background", icon='SCENE_DATA')
        pie.operator("clip.setup_tracking_scene", text="Setup Tracking Scene", icon='SCENE_DATA')

        pie.operator("clip.set_plane", text="Setup Floor", icon='MESH_PLANE')
        pie.operator("clip.set_origin", text="Set Origin", icon='MANIPUL')

        pie.operator("clip.set_axis", text="Set X Axis", icon='AXIS_FRONT').axis = 'X'
        pie.operator("clip.set_axis", text="Set Y Axis", icon='AXIS_SIDE').axis = 'Y'

        pie.operator("clip.set_scale", text="Set Scale", icon='ARROW_LEFTRIGHT')
        pie.operator("wm.call_menu_pie", text="Reconstruction", icon='MESH_DATA').name = "CLIP_PIE_geometry_reconstruction"


class CLIP_PIE_timecontrol_pie(Menu):
    # Time Controls
    bl_label = "Time Control"

    def draw(self, context):
        layout = self.layout
        pie = layout.menu_pie()

        pie.operator("screen.frame_jump", text="Jump to Startframe", icon='TRIA_LEFT').end = False
        pie.operator("screen.frame_jump", text="Jump to Endframe", icon='TRIA_RIGHT').end = True

        pie.operator("clip.frame_jump", text="Start of Track", icon='REW').position = 'PATHSTART'
        pie.operator("clip.frame_jump", text="End of Track", icon='FF').position = 'PATHEND'

        pie.operator("screen.animation_play", text="Playback Backwards", icon='PLAY_REVERSE').reverse = True
        pie.operator("screen.animation_play", text="Playback Forwards", icon='PLAY').reverse = False

        pie.operator("screen.frame_offset", text="Previous Frame", icon='TRIA_LEFT').delta = -1
        pie.operator("screen.frame_offset", text="Next Frame", icon='TRIA_RIGHT').delta = 1


addon_keymaps = []

classes = (
    VIEW3D_manipulator_set,

    VIEW3D_PIE_object_mode,
    VIEW3D_PIE_view,
    #VIEW3D_PIE_view_more,
    VIEW3D_PIE_shade,
    VIEW3D_PIE_manipulator,
    VIEW3D_PIE_pivot,
    VIEW3D_PIE_snap,

    VIEW3D_PIE_create,
    VIEW3D_PIE_select_mode,
    VIEW3D_PIE_snap_origin,
    PIE_screen_helper,
    PIE_origin_helper,

    CLIP_PIE_geometry_reconstruction,
    CLIP_PIE_tracking_pie,
    CLIP_PIE_display_pie,
    CLIP_PIE_proxy_pie,
    CLIP_PIE_marker_pie,
    CLIP_PIE_solver_pie,
    CLIP_PIE_refine_pie,
    CLIP_PIE_reconstruction_pie,
    CLIP_PIE_clipsetup_pie,
    CLIP_PIE_timecontrol_pie,
    )


def register():
    for cls in classes:
        bpy.utils.register_class(cls)

    wm = bpy.context.window_manager

    if wm.keyconfigs.addon:
        km = wm.keyconfigs.addon.keymaps.new(name='Object Non-modal')
        #kmi = km.keymap_items.new('wm.call_menu_pie', 'TAB', 'PRESS')
        kmi = km.keymap_items.new('wm.call_menu_pie', 'BUTTON4MOUSE', 'PRESS')
        kmi.properties.name = 'VIEW3D_PIE_object_mode'
        kmi = km.keymap_items.new('wm.call_menu_pie', 'RIGHTMOUSE', 'PRESS')
        kmi.properties.name = 'VIEW3D_PIE_select_mode'
        kmi = km.keymap_items.new('wm.call_menu_pie', 'X', 'PRESS', shift=True)
        kmi.properties.name = 'VIEW3D_PIE_snap_origin'
        kmi = km.keymap_items.new('wm.call_menu_pie', 'Z', 'PRESS')
        kmi.properties.name = 'VIEW3D_PIE_shade'
        kmi = km.keymap_items.new('wm.call_menu_pie', 'Q', 'PRESS')
        kmi.properties.name = 'VIEW3D_PIE_view'
        #kmi = km.keymap_items.new('wm.call_menu_pie', 'SPACE', 'PRESS', ctrl=True)

        #kmi = km.keymap_items.new('wm.call_menu_pie', 'BUTTON5MOUSE', 'PRESS')
        #kmi.properties.name = 'VIEW3D_PIE_manipulator'
        kmi = km.keymap_items.new('wm.call_menu_pie', 'PERIOD', 'PRESS')
        kmi.properties.name = 'VIEW3D_PIE_pivot'
        kmi = km.keymap_items.new('wm.call_menu_pie', 'RIGHTMOUSE', 'PRESS', shift=True)
        kmi.properties.name = 'VIEW3D_PIE_snap'
        kmi = km.keymap_items.new('view3d.set_selected_origin', 'INSERT', 'PRESS')
        addon_keymaps.append(km)

        km = wm.keyconfigs.addon.keymaps.new(name='Grease Pencil Stroke Edit Mode')
        #kmi = km.keymap_items.new('wm.call_menu_pie', 'TAB', 'PRESS')
        kmi = km.keymap_items.new('wm.call_menu_pie', 'BUTTON4MOUSE', 'PRESS')
        kmi.properties.name = 'VIEW3D_PIE_object_mode'
        addon_keymaps.append(km)

        km = wm.keyconfigs.addon.keymaps.new(name="Clip", space_type='CLIP_EDITOR')
        kmi = km.keymap_items.new("wm.call_menu_pie", 'Q', 'PRESS')
        kmi.properties.name = "CLIP_PIE_marker_pie"
        kmi = km.keymap_items.new("wm.call_menu_pie", 'W', 'PRESS')
        kmi.properties.name = "CLIP_PIE_clipsetup_pie"
        kmi = km.keymap_items.new("wm.call_menu_pie", 'E', 'PRESS')
        kmi.properties.name = "CLIP_PIE_tracking_pie"
        kmi = km.keymap_items.new("wm.call_menu_pie", 'S', 'PRESS', shift=True)
        kmi.properties.name = "CLIP_PIE_solver_pie"
        kmi = km.keymap_items.new("wm.call_menu_pie", 'W', 'PRESS', shift=True)
        kmi.properties.name = "CLIP_PIE_reconstruction_pie"
        addon_keymaps.append(km)

        km = wm.keyconfigs.addon.keymaps.new(name="Frames")
        kmi = km.keymap_items.new("wm.call_menu_pie", 'A', 'PRESS', oskey=True)
        kmi.properties.name = "CLIP_PIE_timecontrol_pie"
        addon_keymaps.append(km)


def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)

    wm = bpy.context.window_manager

    if wm.keyconfigs.addon:
        for km in addon_keymaps:
            for kmi in km.keymap_items:
                km.keymap_items.remove(kmi)

            wm.keyconfigs.addon.keymaps.remove(km)

    addon_keymaps.clear()
