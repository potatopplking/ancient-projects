# v blenderu nutno importovat mathutils
def cube(pos, size, offset=0):
	o = offset
	d = size/2.0
	vertices = [(pos[0]-d,pos[1]-d,pos[2]-d),(pos[0]-d,pos[1]+d,pos[2]-d),(pos[0]-d,pos[1]+d,pos[2]+d),(pos[0]-d,pos[1]-d,pos[2]+d),(pos[0]+d,pos[1]-d,pos[2]-d),(pos[0]+d,pos[1]+d,pos[2]-d),(pos[0]+d,pos[1]+d,pos[2]+d),(pos[0]+d,pos[1]-d,pos[2]+d)]
	faces = [(0+o,3+o,2+o,1+o),(4+o,5+o,6+o,7+o),(1+o,2+o,6+o,5+o),(0+o,4+o,7+o,3+o),(2+o,3+o,7+o,6+o),(0+o,1+o,5+o,4+o)]
	return vertices,faces

def iter_vector(pos,u,v):
	# type(pos,u,v) = mathutils.Vector()
	i = 0
	npos = []
	nu = []
	nv = []
	while i < len(pos):
		w = u[i].cross(v[i])
		w.normalize()
		w *= u[i].length
		# 0
		npos.append(pos[i]+4.0/3*u[i])
		nu.append(1.0/3*u[i])
		nv.append(1.0/3*v[i])
		# 1
		npos.append(pos[i]+4.0/3*v[i])
		nu.append(1.0/3*v[i])
		nv.append(1.0/3*w)
		# 2
		npos.append(pos[i]+4.0/3*w)
		nu.append(1.0/3*w)
		nv.append(-1.0/3*v[i])
		# 3
		npos.append(pos[i]-4.0/3*v[i])
		nu.append(-1.0/3*v[i])
		nv.append(-1.0/3*w)
		# 4
		npos.append(pos[i]-4.0/3*w)
		nu.append(-1.0/3*w)
		nv.append(1.0/3*v[i])
		# temp
		vec_u = 1.0/3*u[i]
		vec_v = 1.0/3*v[i]
		# 5
		npos.append(pos[i]+2*v[i]-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 6
		npos.append(pos[i]+2*v[i]+2*w-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 7
		npos.append(pos[i]+2*w-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 8
		npos.append(pos[i]-2*v[i]+2*w-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 9
		npos.append(pos[i]-2*v[i]-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 10
		npos.append(pos[i]-2*v[i]-2*w-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 11
		npos.append(pos[i]-2*w-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		# 8
		npos.append(pos[i]+2*v[i]-2*w-2.0/3*u[i])
		nu.append(vec_u)
		nv.append(vec_v)
		i += 1
	return npos,nu,nv

def create_cubes(max_lvl):
	pos = [Vector((0.0,0.0,1.5))]
	u = [Vector((0.0,0.0,1.5))]
	v = [Vector((1.5,0.0,0.0))]
	vertices = []
	faces = []
	groups = []
	i = 0
	offset = 0
	while i <= max_lvl:
		j = 0
		temp_group = []
		while j < len(pos):
			v1,f1 = cube(pos[j],u[j].length*2,offset)
			for n in [0,1,2,3,4,5,6,7]:
				temp_group.append(n+offset)
			vertices += v1
			offset += 8
			faces += f1
			j += 1
		pos,u,v = iter_vector(pos,u,v)
		groups.append(temp_group)
		i += 1
	return vertices,faces,groups

def create_scene(lvl):
	frCubesMesh = bpy.data.meshes.new('frCubesMesh')
	frCubes = bpy.data.objects.new('frCubes',frCubesMesh)
	frCubes.location = bpy.context.scene.cursor_location
	bpy.context.scene.objects.link(frCubes)
	v,f,g = create_cubes(lvl)
	frCubesMesh.from_pydata(v,[],f)
	frCubesMesh.update(calc_edges=True)
	for grp in g:
		group = frCubes.vertex_groups.new('frVertexGroup')
		group.add(grp,1.0,'ADD')