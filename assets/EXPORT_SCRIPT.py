import bpy, bmesh, os

obj = bpy.context.object
mesh_name = obj.name
me  = obj.data
uv_layer = me.uv_layers.active

if uv_layer is None:
    raise Exception("Needs UV mapping")

bpy.ops.object.mode_set(mode='EDIT')
bm = bmesh.from_edit_mesh(me)

uv_layer = bm.loops.layers.uv.active
faces_left = set(bm.faces)
islands = []

while faces_left:
    start = faces_left.pop()
    island = {start}
    stack = [start]

    while stack:
        f = stack.pop()
        for e in f.edges:
            if e.seam:
                continue
            for lf in e.link_faces:
                if lf in faces_left:
                    faces_left.remove(lf)
                    island.add(lf)
                    stack.append(lf)

    islands.append(list(island))
    
before = set(bpy.data.objects)

for i, island in enumerate(islands):
    for f in bm.faces: f.select = False
    for f in island:  f.select = True
    bmesh.update_edit_mesh(me)
    bpy.ops.mesh.separate(type='SELECTED')

bpy.ops.object.mode_set(mode='OBJECT')

after = set(bpy.data.objects)

new_objs = list(after - before)

bpy.ops.object.select_all(action='DESELECT')

for o in new_objs:
    o.select = True
    
blend_dir = bpy.path.abspath("//")
EXPORT_PATH = os.path.join(blend_dir, mesh_name + ".obj")
    
bpy.ops.export_scene.obj(
    filepath=EXPORT_PATH,
    use_selection=True,
    use_materials=False,
    group_by_object=True,
    group_by_material=False
)

obj.select = True
bpy.ops.object.join()
bpy.ops.object.mode_set(mode='EDIT')
bpy.ops.mesh.select_all(action='TOGGLE')
bpy.ops.mesh.remove_doubles()
bpy.ops.object.mode_set(mode='OBJECT')