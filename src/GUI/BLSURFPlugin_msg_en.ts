<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="en_US">
<context>
    <name>@default</name>
    <message>
        <source>BLSURF_MESH_TYPE</source>
        <translation>Type of mesh</translation>
    </message>
    <message>
        <source>BLSURF_PHY_MESH</source>
        <translation>Physical Mesh</translation>
    </message>
    <message>
        <source>BLSURF_PHY_MESH_TOOLTIP</source>
        <translation>&lt;b&gt;&lt;u&gt;Physical size mode&lt;/u&gt;&lt;/b&gt;&lt;br&gt;&lt;br&gt;
- &quot;Global size&quot;: the physical size is global and the mesher will use the global physical size provided.&lt;br&gt;
- &quot;Local size&quot;: the size is locally computed, on curves and surfaces, using the associated physical sizemap functions.&lt;br&gt;
- &quot;None&quot;: no physical size will be computed.</translation>
    </message>
    <message>
        <source>BLSURF_GEOM_MESH</source>
        <translation>Geometrical Mesh</translation>
    </message>
    <message>
        <source>BLSURF_GEOM_MESH_TOOLTIP</source>
        <translation>&lt;b&gt;&lt;u&gt;Geometric size mode&lt;/u&gt;&lt;/b&gt;&lt;br&gt;&lt;br&gt;
- &quot;Global size&quot;: the geometrical size is global and the mesher will use the geometrical approximation provided by the geometrical parameters..&lt;br&gt;
- &quot;None&quot;: no geometrical sizes will be computed.</translation>
    </message>
    <message>
        <source>BLSURF_DEFAULT_USER</source>
        <translation>None</translation>
    </message>
    <message>
        <source>GLOBAL_SIZE</source>
        <translation>Global size</translation>
    </message>
    <message>
        <source>LOCAL_SIZE</source>
        <translation>Local size</translation>
    </message>
    <message>
        <source>BLSURF_MAIN_PARAMETERS</source>
        <translation>Main parameters</translation>
    </message>
    <message>
        <source>BLSURF_HPHYDEF</source>
        <translation>User Size</translation>
    </message>
    <message>
        <source>BLSURF_HPHYDEF_TOOLTIP</source>
        <translation>Defines the constant global size.&lt;br&gt;
The default computed value is &lt;em&gt;diag&lt;/em&gt;/&lt;em&gt;ratio&lt;/em&gt;, where &lt;em&gt;ratio&lt;/em&gt; is defined in the preferences.</translation>
    </message>
    <message>
        <source>BLSURF_MINSIZE</source>
        <translation>Min Size</translation>
    </message>
    <message>
        <source>BLSURF_MINSIZE_TOOLTIP</source>
        <translation>Defines the global minimum cell size desired.&lt;br&gt;
The default computed value is &lt;em&gt;diag&lt;/em&gt;/1000.</translation>
    </message>
    <message>
        <source>BLSURF_MAXSIZE</source>
        <translation>Max Size</translation>
    </message>
    <message>
        <source>BLSURF_MAXSIZE_TOOLTIP</source>
        <translation>Defines the global maximum cell size desired.&lt;br&gt;
The default computed value is &lt;em&gt;diag&lt;/em&gt;/5.</translation>
    </message>
    <message>
        <source>BLSURF_SIZE_REL</source>
        <translation>Relative value</translation>
    </message>
    <message>
        <source>BLSURF_SIZE_REL_TOOLTIP</source>
        <translation>The value is relative to &lt;em&gt;diag&lt;/em&gt;</translation>
    </message>
    <message>
        <source>BLSURF_GRADATION</source>
        <translation>Gradation</translation>
    </message>
    <message>
        <source>BLSURF_GRADATION_TOOLTIP</source>
        <translation>Maximum ratio between the lengths of two adjacent edges.</translation>
    </message>
    <message>
        <source>BLSURF_ALLOW_QUADRANGLES</source>
        <translation>Allow Quadrangles</translation>
    </message>
    <message>
        <source>BLSURF_GEOMETRICAL_PARAMETERS</source>
        <translation>Geometrical parameters</translation>
    </message>
    <message>
        <source>BLSURF_ANGLE_MESH</source>
        <translation>Mesh angle</translation>
    </message>
    <message>
        <source>BLSURF_ANGLE_MESH_TOOLTIP</source>
        <translation>Limiting angle between the plane of a triangle of the mesh and each of the tangent planes at the three vertices.&lt;br&gt;
The smaller this angle is, the closer the mesh is to the exact surface, and the denser the resulting mesh is.</translation>
    </message>
    <message>
        <source>BLSURF_CHORDAL_ERROR</source>
        <translation>Mesh distance</translation>
    </message>
    <message>
        <source>BLSURF_CHORDAL_TOOLTIP</source>
        <translation>Maximum desired distance between a triangle and its supporting CAD surface.&lt;br&gt;
The smaller this distance is, the closer the mesh is to the exact surface (only available in isotropic meshing).</translation>
    </message>
    <message>
        <source>BLSURF_OTHER_PARAMETERS</source>
        <translation>Other parameters</translation>
    </message>
    <message>
        <source>BLSURF_ANISOTROPIC</source>
        <translation>Anisotropic</translation>
    </message>
    <message>
        <source>BLSURF_ANISOTROPIC_TOOLTIP</source>
        <translation>If checked, this parameter defines the maximum anisotropic ratio of the metric governing the anisotropic meshing process.&lt;br&gt;
          The default value of 0 means that the metric (and thus the generated elements) can be arbitrarily stretched.</translation>
    </message>
    <message>
        <source>BLSURF_REMOVE_TINY_EDGES</source>
        <translation>Remove tiny edges</translation>
    </message>
    <message>
        <source>BLSURF_REMOVE_TINY_EDGES_TOOLTIP</source>
        <translation>If checked, this parameter defines the minimal length under which an edge is considered to be a tiny one.</translation>
    </message>
    <message>
        <source>BLSURF_REMOVE_SLIVERS</source>
        <translation>Remove bad elements</translation>
    </message>
    <message>
        <source>BLSURF_REMOVE_SLIVERS_TOOLTIP</source>
        <translation>If checked, this parameter defines the aspect ratio triggering the "bad element” classification.</translation>
    </message>
    <message>
        <source>BLSURF_OPTIMISATION</source>
        <translation>Mesh optimisation</translation>
    </message>
    <message>
        <source>BLSURF_OPTIMISATION_TOOLTIP</source>
        <translation>If checked, MeshGems-CADSurf will optimize the mesh in order to get better shaped elements.</translation>
    </message>
    <message>
        <source>BLSURF_ELEMENT_ORDER</source>
        <translation>Quadratic mesh</translation>
    </message>
    <message>
        <source>BLSURF_HYPOTHESIS</source>
        <translation>MG-CADSurf</translation>
    </message>
    <message>
        <source>BLSURF_ADV_ARGS</source>
        <translation>Advanced</translation>
    </message>
    <message>
        <source>BLSURF_TITLE</source>
        <translation>Hypothesis Construction</translation>
    </message>
    <message>
        <source>BLSURF_TOPOLOGY</source>
        <translation>Topology</translation>
    </message>
    <message>
        <source>BLSURF_TOPOLOGY_CAD</source>
        <translation>From CAD</translation>
    </message>
    <message>
        <source>BLSURF_TOPOLOGY_PROCESS</source>
        <translation>Pre-process</translation>
    </message>
    <message>
        <source>BLSURF_TOPOLOGY_PROCESS2</source>
        <translation>Pre-process++</translation>
    </message>
    <message>
        <source>BLSURF_TOPOLOGY_PRECAD</source>
        <translation>PreCAD</translation>
    </message>
    <message>
        <source>BLSURF_VERBOSITY</source>
        <translation>Verbosity level</translation>
    </message>
    <message>
        <source>OBLIGATORY_VALUE</source>
        <translation>(Mandatory value)</translation>
    </message>
    <message>
        <source>OPTION_TYPE_COLUMN</source>
        <translation>Type</translation>
    </message>
    <message>
        <source>OPTION_NAME_COLUMN</source>
        <translation>Option</translation>
    </message>
    <message>
        <source>OPTION_VALUE_COLUMN</source>
        <translation>Value</translation>
    </message>
    <message>
        <source>OPTION_MENU_BLSURF</source>
        <translation>MG-CADSurf</translation>
    </message>
    <message>
        <source>OPTION_MENU_PRECAD</source>
        <translation>PreCAD</translation>
    </message>
    <message>
        <source>BLSURF_ADD_OPTION</source>
        <translation>Add option</translation>
    </message>
    <message>
        <source>BLSURF_REMOVE_OPTION</source>
        <translation>Clear option</translation>
    </message>
    <message>
        <source>BLSURF_GMF_FILE</source>
        <translation>Export GMF</translation>
    </message>
    <message>
        <source>BLSURF_GMF_MODE</source>
        <translation>Binary</translation>
    </message>
    <message>
        <source>BLSURF_GMF_FILE_DIALOG</source>
        <translation>Select GMF file...</translation>
    </message>
    <message>
        <source>BLSURF_GMF_FILE_FORMAT</source>
        <translation>GMF File (*.mesh *.meshb)</translation>
    </message>
    <message>
        <source>BLSURF_PRECAD_GROUP</source>
        <translation>PreCAD options</translation>
    </message>
    <message>
        <source>BLSURF_PRECAD_MERGE_EDGES</source>
        <translation>Merge edges</translation>
    </message>
    <message>
        <source>BLSURF_PRECAD_PROCESS_3D_TOPOLOGY</source>
        <translation>Process 3D topology</translation>
    </message>
    <message>
        <source>BLSURF_PRECAD_DISCARD_INPUT</source>
        <translation>Discard input topology</translation>
    </message>
    <message>
        <source>SMP_ENTRY_COLUMN</source>
        <translation>Entry</translation>
    </message>
    <message>
        <source>SMP_NAME_COLUMN</source>
        <translation>Name</translation>
    </message>
    <message>
        <source>SMP_SIZEMAP_COLUMN</source>
        <translation>Local size</translation>
    </message>
    <message>
        <source>BLSURF_SM_SURFACE</source>
        <translation>On face (or group)</translation>
    </message>
    <message>
        <source>BLSURF_SM_EDGE</source>
        <translation>On edge (or group)</translation>
    </message>
    <message>
        <source>BLSURF_SM_POINT</source>
        <translation>On point (or group)</translation>
    </message>
    <message>
        <source>BLSURF_SM_ATTRACTOR</source>
        <translation>Add Attractor</translation>
    </message>
    <message>
        <source>BLSURF_SM_STD_TAB</source>
        <translation>Simple map</translation>
    </message>
    <message>
        <source>BLSURF_SM_ATT_TAB</source>
        <translation>Advanced</translation>
    </message>
    <message>
        <source>BLSURF_SM_PARAMS</source>
        <translation>Parameters</translation>
    </message>
    <message>
        <source>BLSURF_ATTRACTOR</source>
        <translation>Attractor</translation>
    </message>
    <message>
        <source>BLSURF_CONST_SIZE</source>
        <translation>Constant size near shape</translation>
    </message>
    <message>
        <source>BLSURF_ATT_DIST</source>
        <translation>Influence dist.</translation>
    </message>
    <message>
        <source>BLSURF_ATT_RADIUS</source>
        <translation>Constant over</translation>
    </message>
    <message>
        <source>BLSURF_SM_SIZE</source>
        <translation>Local Size</translation>
    </message>
    <message>
        <source>BLSURF_SM_DIST</source>
        <translation>Distance</translation>
    </message>
    <message>
        <source>BLS_SEL_SHAPE</source>
        <translation>Select a shape</translation>
    </message>
    <message>
        <source>BLS_SEL_VERTICES</source>
        <translation>Select vertices</translation>
    </message>
    <message>
        <source>BLS_SEL_FACE</source>
        <translation>Select a face</translation>
    </message>
    <message>
        <source>BLS_SEL_FACES</source>
        <translation>Select faces</translation>
    </message>
    <message>
        <source>BLS_SEL_ATTRACTOR</source>
        <translation>Select the attractor</translation>
    </message>
    <message>
        <source>BLSURF_SM_ADD</source>
        <translation>Add</translation>
    </message>
    <message>
        <source>BLSURF_SM_MODIFY</source>
        <translation>Modify</translation>
    </message>
    <message>
        <source>BLSURF_SM_REMOVE</source>
        <translation>Remove</translation>
    </message>
    <message>
        <source>BLSURF_SM_SURF_VALUE</source>
        <translation>Size on face(s)</translation>
    </message>
    <message>
        <source>BLSURF_SM_EDGE_VALUE</source>
        <translation>Size on edge(s)</translation>
    </message>
    <message>
        <source>BLSURF_SM_POINT_VALUE</source>
        <translation>Size on point(s)</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER</source>
        <translation>Enforced vertices</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_FACE_ENTRY_COLUMN</source>
        <translation>Face Entry</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_NAME_COLUMN</source>
        <translation>Name</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_X_COLUMN</source>
        <translation>X</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_Y_COLUMN</source>
        <translation>Y</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_Z_COLUMN</source>
        <translation>Z</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_ENTRY_COLUMN</source>
        <translation>Vertex Entry</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_GROUP_COLUMN</source>
        <translation>Group</translation>
    </message>
    <message>
        <source>BLSURF_ENF_SELECT_FACE</source>
        <translation>Select a face</translation>
    </message>
    <message>
        <source>BLSURF_ENF_SELECT_VERTEX</source>
        <translation>Select a vertex</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_X_LABEL</source>
        <translation>X:</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_Y_LABEL</source>
        <translation>Y:</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_Z_LABEL</source>
        <translation>Z:</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_GROUP_LABEL</source>
        <translation>Group:</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_VERTEX</source>
        <translation>Add</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_REMOVE</source>
        <translation>Remove</translation>
    </message>
    <message>
        <source>BLSURF_ENF_VER_INTERNAL_VERTICES</source>
        <translation>Use internal vertices of all faces</translation>
    </message>
    <message>
        <source>ZERO_VALUE_OF</source>
        <translation>Invalid value (zero) of "%1"</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY</source>
        <translation>Periodicity</translation>
    </message>
    <message>
        <source>BLSURF_PRECAD_PERIODICITY</source>
        <translation>Periodicity association (using PreCAD)</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_ON_FACE</source>
        <translation>On face</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_ON_EDGE</source>
        <translation>On edge only</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_OBJ_SOURCE_COLUMN</source>
        <translation>Source</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_OBJ_TARGET_COLUMN</source>
        <translation>Target</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P1_SOURCE_COLUMN</source>
        <translation>P1</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P2_SOURCE_COLUMN</source>
        <translation>P2</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P3_SOURCE_COLUMN</source>
        <translation>P3</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P1_TARGET_COLUMN</source>
        <translation>P1</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P2_TARGET_COLUMN</source>
        <translation>P2</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P3_TARGET_COLUMN</source>
        <translation>P3</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_MAIN_SOURCE</source>
        <translation>Source</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_MAIN_TARGET</source>
        <translation>Target</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_SOURCE</source>
        <translation>Source</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_TARGET</source>
        <translation>Target</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_WITH_VERTICES</source>
        <translation>Transformation vertices</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P1_SOURCE</source>
        <translation>P1</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P2_SOURCE</source>
        <translation>P2</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P3_SOURCE</source>
        <translation>P3</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P1_TARGET</source>
        <translation>P1</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P2_TARGET</source>
        <translation>P2</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_P3_TARGET</source>
        <translation>P3</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_ADD</source>
        <translation>Add</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_REMOVE</source>
        <translation>Remove</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_WRONG_NUMBER_OF_VERTICES</source>
        <translation>Wrong number of vertices provided</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_SELECT_FACE</source>
        <translation>Face</translation>
    </message>
    <message>
        <source>BLSURF_PERIODICITY_SELECT_EDGE</source>
        <translation>Edge</translation>
    </message>
</context>
</TS>
