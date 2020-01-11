#-------------------------------------------------------------------------------
# Toolkit
#-------------------------------------------------------------------------------

fips_begin_module(toolkitutil)
	fips_vs_warning_level(3)
    fips_include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CODE_ROOT}/foundation ${CODE_ROOT}/render ${CODE_ROOT}/)
    fips_deps(foundation render ${FBX_LIB})
    if (N_BUILD_NVTT)
		fips_deps(nvtt)
		ADD_DEFINITIONS(-DUSE_NVTT)
    endif()
    	fips_files(			
			assetfile.cc
			assetfile.h
			assetregistry.cc
			assetregistry.h
			assetupdater.cc
			assetupdater.h
			audioexporter.cc
			audioexporter.h
			binarymodelwriter.cc
			binarymodelwriter.h			
			modelwriter.cc
			modelwriter.h									
			#resourcedictionarybuilder.cc
			#resourcedictionarybuilder.h
			scenenode.cc
			scenenode.h
			scenenodetree.cc
			scenenodetree.h			
			xmlmodelwriter.cc
			xmlmodelwriter.h
		)

		fips_dir(asset)
			fips_files(
				assetexporter.cc
				assetexporter.h
			)
		fips_dir(animutil)
			fips_files(
				animbuilder.cc
				animbuilder.h
				animbuilderclip.cc
				animbuilderclip.h
				animbuildercurve.cc
				animbuildercurve.h
				animbuilderloader.cc
				animbuilderloader.h
				animbuildersaver.cc
				animbuildersaver.h
				animconverter.cc
				animconverter.h
			)
		fips_dir(meshutil)
			fips_files(
				meshbuilder.cc
				meshbuilder.h
				meshbuildergroup.cc
				meshbuildergroup.h
				meshbuildersaver.cc
				meshbuildersaver.h
				meshbuildertriangle.cc
				meshbuildertriangle.h
				meshbuildervertex.cc
				meshbuildervertex.h
			)
		fips_dir(n3util)
			fips_files(
				n3modeldata.h
				n3writer.cc
				n3writer.h
				n3xmlexporter.cc
				n3xmlexporter.h
				n3xmlextractor.cc
				n3xmlextractor.h
				n3xmlmodifier.cc
				n3xmlmodifier.h
			)
		fips_dir(skeletonutil)
			fips_files(
				skeletonbuilder.h
				skeletonbuildersaver.cc
				skeletonbuildersaver.h
			)
	IF(NOT N_MINIMAL_TOOLKIT)
		#fips_dir(posteffect)
		#	fips_files(
		#		posteffectexporter.cc
		#		posteffectexporter.h
		#		posteffectparser.cc
		#		posteffectparser.h
		#	)
	ENDIF()
		IF(N_BUILD_NVTT)
			SET(SRCSX_PL
				nvtttextureconversionjob.cc
				nvtttextureconversionjob.h
				)
		ELSE()
			SET(SRCSX_PL
				win32textureconversionjob.cc
				win32textureconversionjob.h
				)
		ENDIF()                 
	IF(NOT N_MINIMAL_TOOLKIT)
		fips_dir(texutil)
			fips_files(
				# cubefilterer.cc
				# cubefilterer.h
				imageconverter.cc
				imageconverter.h
				textureattrs.cc
				textureattrs.h
				textureattrtable.cc
				textureattrtable.h
				textureconversionjob.cc
				textureconversionjob.h
				textureconverter.cc
				textureconverter.h
				${SRCSX_PL}
			)
	ENDIF()		
		fips_dir(fbx)
			fips_files(
				nfbxexporter.cc	
				nfbxexporter.h
				nfbxscenewriter.cc
				nfbxscenewriter.h
			)

			fips_dir(fbx/helpers)
				fips_files(
					animsplitterhelper.cc
					animsplitterhelper.h
					batchattributes.cc
					batchattributes.h
					skinhelper.cc
					skinhelper.h
				)
			fips_dir(fbx/node)
				fips_files(
					nfbxjointnode.cc
					nfbxjointnode.h
					nfbxlightnode.cc
					nfbxlightnode.h
					nfbxmeshnode.cc
					nfbxmeshnode.h
					nfbxnode.cc
					nfbxnode.h
					nfbxscene.cc
					nfbxscene.h
					nfbxtransformnode.cc
					nfbxtransformnode.h
				)
			fips_dir(fbx/character)
				fips_files(
					skinfragment.cc
					skinfragment.h
					skinpartitioner.cc
					skinpartitioner.h
				)
IF(NOT N_MINIMAL_TOOLKIT)
		#fips_dir(game)
		#	fips_files(
		#		gameexporter.cc
		#		gameexporter.h
		#		leveldbwriter.cc
		#		leveldbwriter.h
		#		levelparser.cc
		#		levelparser.h
		#	)

		#fips_dir(gui)
		#	fips_files(
		#		guiexporter.cc
		#		guiexporter.h
		#	)
ENDIF()			
		fips_dir(modelutil)
			fips_files(
				clip.cc
				clip.h
				clipevent.cc
				clipevent.h
				modelattributes.cc
				modelattributes.h
				modelbuilder.cc
				modelbuilder.h
				modelconstants.cc
				modelconstants.h
				modeldatabase.cc
			    modeldatabase.h
				#modelphysics.cc
				#modelphysics.h
				take.cc
				take.h
			)					
		fips_dir(surface)
			fips_files(
				surfaceexporter.cc
				surfaceexporter.h
			)
fips_end_module()
# target_link_libraries(toolkitutil PUBLIC ${FBX_LIB})
target_compile_definitions(toolkitutil PUBLIC $<$<BOOL:${N_BUILD_NVTT}>:NVTT_EXPORTS> FBXSDK_SHARED)
target_include_directories(toolkitutil PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})