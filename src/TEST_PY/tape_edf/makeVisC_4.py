
# Hexa : Creation de VisC_4

import hexablock
from hexa_tape import *

# ============================================================== build
def build () :
   name  = 'VisC_4'
   geom  = 'True'
   doc   = add_document (name, geom)

   vertex_07 = doc.addVertex (217.362,459.739,61.75)
   vertex_08 = doc.addVertex (224.112,459.739,61.75)
   vertex_10 = doc.addVertex (208.112,459.739,61.75)
   vertex_101 = doc.addVertex (217.362,459.739,37.75)
   vertex_106 = doc.addVertex (208.112,459.739,37.75)
   vertex_111 = doc.addVertex (214.862,459.739,37.75)
   vertex_12 = doc.addVertex (214.862,459.739,61.75)
   vertex_137 = doc.addVertex (223.762,459.739,-29.25)
   vertex_138 = doc.addVertex (217.362,459.739,-29.25)
   vertex_140 = doc.addVertex (224.812,459.739,-25.25)
   vertex_142 = doc.addVertex (217.362,459.739,-25.25)
   vertex_147 = doc.addVertex (214.862,459.739,-29.25)
   vertex_149 = doc.addVertex (208.462,459.739,-29.25)
   vertex_154 = doc.addVertex (207.412,459.739,-25.25)
   vertex_159 = doc.addVertex (214.862,459.739,-25.25)
   vertex_17 = doc.addVertex (224.112,459.739,48.25)
   vertex_19 = doc.addVertex (217.362,459.739,48.25)
   vertex_24 = doc.addVertex (208.112,459.739,48.25)
   vertex_259 = doc.addVertex (230.362,459.739,61.75)
   vertex_261 = doc.addVertex (201.862,459.739,61.75)
   vertex_266 = doc.addVertex (230.362,459.739,48.25)
   vertex_271 = doc.addVertex (201.862,459.739,48.25)
   vertex_29 = doc.addVertex (214.862,459.739,48.25)
   vertex_293 = doc.addVertex (224.262,459.739,-29.25)
   vertex_295 = doc.addVertex (225.312,459.739,-25.25)
   vertex_300 = doc.addVertex (207.962,459.739,-29.25)
   vertex_305 = doc.addVertex (206.912,459.739,-25.25)
   vertex_317 = doc.addVertex (225.312,459.739,-3.96131)
   vertex_322 = doc.addVertex (206.912,459.739,-3.96131)
   vertex_351 = doc.addVertex (237.362,459.739,37.75)
   vertex_353 = doc.addVertex (194.862,459.739,37.75)
   vertex_358 = doc.addVertex (237.362,459.739,29.75)
   vertex_363 = doc.addVertex (194.862,459.739,29.75)
   vertex_41 = doc.addVertex (217.362,459.739,-3.75)
   vertex_42 = doc.addVertex (224.812,459.739,-3.75)
   vertex_44 = doc.addVertex (224.812,459.739,13)
   vertex_46 = doc.addVertex (217.362,459.739,13)
   vertex_51 = doc.addVertex (214.862,459.739,-3.75)
   vertex_53 = doc.addVertex (207.412,459.739,-3.75)
   vertex_58 = doc.addVertex (207.412,459.739,13)
   vertex_63 = doc.addVertex (214.862,459.739,13)
   vertex_75 = doc.addVertex (224.812,459.739,29.75)
   vertex_77 = doc.addVertex (217.362,459.739,29.75)
   vertex_82 = doc.addVertex (207.412,459.739,29.75)
   vertex_87 = doc.addVertex (214.862,459.739,29.75)
   vertex_99 = doc.addVertex (224.112,459.739,37.75)

   edge_06 = add_edge (6, vertex_07, vertex_08)
   edge_09 = add_edge (9, vertex_08, vertex_10)
   edge_100 = add_edge (100, vertex_101, vertex_99)
   edge_102 = add_edge (102, vertex_77, vertex_101)
   edge_105 = add_edge (105, vertex_99, vertex_106)
   edge_107 = add_edge (107, vertex_106, vertex_82)
   edge_11 = add_edge (11, vertex_10, vertex_12)
   edge_110 = add_edge (110, vertex_101, vertex_111)
   edge_112 = add_edge (112, vertex_106, vertex_111)
   edge_115 = add_edge (115, vertex_87, vertex_111)
   edge_122 = add_edge (122, vertex_17, vertex_99)
   edge_123 = add_edge (123, vertex_101, vertex_19)
   edge_126 = add_edge (126, vertex_24, vertex_106)
   edge_129 = add_edge (129, vertex_111, vertex_29)
   edge_13 = add_edge (13, vertex_07, vertex_12)
   edge_136 = add_edge (136, vertex_137, vertex_138)
   edge_139 = add_edge (139, vertex_140, vertex_137)
   edge_141 = add_edge (141, vertex_142, vertex_140)
   edge_143 = add_edge (143, vertex_138, vertex_142)
   edge_146 = add_edge (146, vertex_138, vertex_147)
   edge_148 = add_edge (148, vertex_147, vertex_149)
   edge_150 = add_edge (150, vertex_137, vertex_149)
   edge_153 = add_edge (153, vertex_149, vertex_154)
   edge_155 = add_edge (155, vertex_140, vertex_154)
   edge_158 = add_edge (158, vertex_142, vertex_159)
   edge_16 = add_edge (16, vertex_17, vertex_08)
   edge_160 = add_edge (160, vertex_154, vertex_159)
   edge_163 = add_edge (163, vertex_147, vertex_159)
   edge_170 = add_edge (170, vertex_42, vertex_140)
   edge_171 = add_edge (171, vertex_142, vertex_41)
   edge_174 = add_edge (174, vertex_53, vertex_154)
   edge_177 = add_edge (177, vertex_159, vertex_51)
   edge_18 = add_edge (18, vertex_19, vertex_07)
   edge_184 = add_edge (184, vertex_12, vertex_07)
   edge_185 = add_edge (185, vertex_10, vertex_08)
   edge_188 = add_edge (188, vertex_29, vertex_19)
   edge_191 = add_edge (191, vertex_24, vertex_17)
   edge_198 = add_edge (198, vertex_159, vertex_142)
   edge_199 = add_edge (199, vertex_147, vertex_138)
   edge_20 = add_edge (20, vertex_19, vertex_17)
   edge_202 = add_edge (202, vertex_154, vertex_140)
   edge_205 = add_edge (205, vertex_149, vertex_137)
   edge_212 = add_edge (212, vertex_51, vertex_41)
   edge_215 = add_edge (215, vertex_53, vertex_42)
   edge_222 = add_edge (222, vertex_63, vertex_46)
   edge_225 = add_edge (225, vertex_44, vertex_58)
   edge_23 = add_edge (23, vertex_24, vertex_10)
   edge_232 = add_edge (232, vertex_87, vertex_77)
   edge_235 = add_edge (235, vertex_82, vertex_75)
   edge_242 = add_edge (242, vertex_111, vertex_101)
   edge_245 = add_edge (245, vertex_106, vertex_99)
   edge_25 = add_edge (25, vertex_17, vertex_24)
   edge_258 = add_edge (258, vertex_08, vertex_259)
   edge_260 = add_edge (260, vertex_259, vertex_261)
   edge_262 = add_edge (262, vertex_261, vertex_10)
   edge_265 = add_edge (265, vertex_266, vertex_17)
   edge_267 = add_edge (267, vertex_259, vertex_266)
   edge_270 = add_edge (270, vertex_266, vertex_271)
   edge_272 = add_edge (272, vertex_261, vertex_271)
   edge_275 = add_edge (275, vertex_24, vertex_271)
   edge_28 = add_edge (28, vertex_24, vertex_29)
   edge_282 = add_edge (282, vertex_261, vertex_259)
   edge_285 = add_edge (285, vertex_271, vertex_266)
   edge_292 = add_edge (292, vertex_293, vertex_137)
   edge_294 = add_edge (294, vertex_295, vertex_293)
   edge_296 = add_edge (296, vertex_140, vertex_295)
   edge_299 = add_edge (299, vertex_149, vertex_300)
   edge_30 = add_edge (30, vertex_29, vertex_12)
   edge_301 = add_edge (301, vertex_293, vertex_300)
   edge_304 = add_edge (304, vertex_305, vertex_154)
   edge_306 = add_edge (306, vertex_300, vertex_305)
   edge_309 = add_edge (309, vertex_295, vertex_305)
   edge_316 = add_edge (316, vertex_317, vertex_295)
   edge_318 = add_edge (318, vertex_42, vertex_317)
   edge_321 = add_edge (321, vertex_322, vertex_305)
   edge_323 = add_edge (323, vertex_317, vertex_322)
   edge_326 = add_edge (326, vertex_53, vertex_322)
   edge_33 = add_edge (33, vertex_19, vertex_29)
   edge_333 = add_edge (333, vertex_305, vertex_295)
   edge_336 = add_edge (336, vertex_300, vertex_293)
   edge_343 = add_edge (343, vertex_322, vertex_317)
   edge_350 = add_edge (350, vertex_99, vertex_351)
   edge_352 = add_edge (352, vertex_351, vertex_353)
   edge_354 = add_edge (354, vertex_353, vertex_106)
   edge_357 = add_edge (357, vertex_358, vertex_75)
   edge_359 = add_edge (359, vertex_351, vertex_358)
   edge_362 = add_edge (362, vertex_358, vertex_363)
   edge_364 = add_edge (364, vertex_353, vertex_363)
   edge_367 = add_edge (367, vertex_82, vertex_363)
   edge_374 = add_edge (374, vertex_353, vertex_351)
   edge_377 = add_edge (377, vertex_363, vertex_358)
   edge_40 = add_edge (40, vertex_41, vertex_42)
   edge_43 = add_edge (43, vertex_44, vertex_42)
   edge_45 = add_edge (45, vertex_46, vertex_44)
   edge_47 = add_edge (47, vertex_41, vertex_46)
   edge_50 = add_edge (50, vertex_41, vertex_51)
   edge_52 = add_edge (52, vertex_53, vertex_51)
   edge_54 = add_edge (54, vertex_42, vertex_53)
   edge_57 = add_edge (57, vertex_58, vertex_53)
   edge_59 = add_edge (59, vertex_58, vertex_44)
   edge_62 = add_edge (62, vertex_46, vertex_63)
   edge_64 = add_edge (64, vertex_58, vertex_63)
   edge_67 = add_edge (67, vertex_51, vertex_63)
   edge_74 = add_edge (74, vertex_75, vertex_44)
   edge_76 = add_edge (76, vertex_77, vertex_75)
   edge_78 = add_edge (78, vertex_46, vertex_77)
   edge_81 = add_edge (81, vertex_82, vertex_58)
   edge_83 = add_edge (83, vertex_75, vertex_82)
   edge_86 = add_edge (86, vertex_77, vertex_87)
   edge_88 = add_edge (88, vertex_82, vertex_87)
   edge_91 = add_edge (91, vertex_63, vertex_87)
   edge_98 = add_edge (98, vertex_75, vertex_99)

   face_04 = doc.addQuad (edge_06, edge_09, edge_11, edge_13)
   face_103 = doc.addQuad (edge_98, edge_105, edge_107, edge_83)
   face_108 = doc.addQuad (edge_100, edge_110, edge_112, edge_105)
   face_113 = doc.addQuad (edge_102, edge_110, edge_115, edge_86)
   face_116 = doc.addQuad (edge_107, edge_88, edge_115, edge_112)
   face_120 = doc.addQuad (edge_100, edge_122, edge_20, edge_123)
   face_124 = doc.addQuad (edge_122, edge_105, edge_126, edge_25)
   face_127 = doc.addQuad (edge_123, edge_33, edge_129, edge_110)
   face_130 = doc.addQuad (edge_126, edge_112, edge_129, edge_28)
   face_134 = doc.addQuad (edge_136, edge_139, edge_141, edge_143)
   face_14 = doc.addQuad (edge_16, edge_06, edge_18, edge_20)
   face_144 = doc.addQuad (edge_136, edge_146, edge_148, edge_150)
   face_151 = doc.addQuad (edge_139, edge_150, edge_153, edge_155)
   face_156 = doc.addQuad (edge_141, edge_158, edge_160, edge_155)
   face_161 = doc.addQuad (edge_143, edge_158, edge_163, edge_146)
   face_164 = doc.addQuad (edge_153, edge_148, edge_163, edge_160)
   face_168 = doc.addQuad (edge_141, edge_170, edge_40, edge_171)
   face_172 = doc.addQuad (edge_170, edge_155, edge_174, edge_54)
   face_175 = doc.addQuad (edge_171, edge_50, edge_177, edge_158)
   face_178 = doc.addQuad (edge_174, edge_160, edge_177, edge_52)
   face_182 = doc.addQuad (edge_184, edge_11, edge_185, edge_06)
   face_186 = doc.addQuad (edge_30, edge_184, edge_18, edge_188)
   face_189 = doc.addQuad (edge_191, edge_23, edge_185, edge_16)
   face_192 = doc.addQuad (edge_191, edge_28, edge_188, edge_20)
   face_196 = doc.addQuad (edge_163, edge_198, edge_143, edge_199)
   face_200 = doc.addQuad (edge_202, edge_160, edge_198, edge_141)
   face_203 = doc.addQuad (edge_205, edge_148, edge_199, edge_136)
   face_206 = doc.addQuad (edge_202, edge_153, edge_205, edge_139)
   face_21 = doc.addQuad (edge_16, edge_09, edge_23, edge_25)
   face_210 = doc.addQuad (edge_177, edge_212, edge_171, edge_198)
   face_213 = doc.addQuad (edge_215, edge_52, edge_212, edge_40)
   face_216 = doc.addQuad (edge_215, edge_174, edge_202, edge_170)
   face_220 = doc.addQuad (edge_67, edge_222, edge_47, edge_212)
   face_223 = doc.addQuad (edge_45, edge_225, edge_64, edge_222)
   face_226 = doc.addQuad (edge_57, edge_215, edge_43, edge_225)
   face_230 = doc.addQuad (edge_91, edge_232, edge_78, edge_222)
   face_233 = doc.addQuad (edge_235, edge_88, edge_232, edge_76)
   face_236 = doc.addQuad (edge_81, edge_225, edge_74, edge_235)
   face_240 = doc.addQuad (edge_115, edge_242, edge_102, edge_232)
   face_243 = doc.addQuad (edge_245, edge_112, edge_242, edge_100)
   face_246 = doc.addQuad (edge_235, edge_107, edge_245, edge_98)
   face_250 = doc.addQuad (edge_129, edge_188, edge_123, edge_242)
   face_252 = doc.addQuad (edge_191, edge_126, edge_245, edge_122)
   face_256 = doc.addQuad (edge_258, edge_260, edge_262, edge_09)
   face_26 = doc.addQuad (edge_28, edge_30, edge_11, edge_23)
   face_263 = doc.addQuad (edge_265, edge_267, edge_258, edge_16)
   face_268 = doc.addQuad (edge_267, edge_270, edge_272, edge_260)
   face_273 = doc.addQuad (edge_262, edge_272, edge_275, edge_23)
   face_276 = doc.addQuad (edge_265, edge_25, edge_275, edge_270)
   face_280 = doc.addQuad (edge_185, edge_262, edge_282, edge_258)
   face_283 = doc.addQuad (edge_282, edge_272, edge_285, edge_267)
   face_286 = doc.addQuad (edge_285, edge_275, edge_191, edge_265)
   face_290 = doc.addQuad (edge_139, edge_292, edge_294, edge_296)
   face_297 = doc.addQuad (edge_292, edge_150, edge_299, edge_301)
   face_302 = doc.addQuad (edge_153, edge_304, edge_306, edge_299)
   face_307 = doc.addQuad (edge_155, edge_304, edge_309, edge_296)
   face_31 = doc.addQuad (edge_18, edge_13, edge_30, edge_33)
   face_310 = doc.addQuad (edge_294, edge_301, edge_306, edge_309)
   face_314 = doc.addQuad (edge_170, edge_296, edge_316, edge_318)
   face_319 = doc.addQuad (edge_316, edge_309, edge_321, edge_323)
   face_324 = doc.addQuad (edge_318, edge_323, edge_326, edge_54)
   face_327 = doc.addQuad (edge_174, edge_326, edge_321, edge_304)
   face_331 = doc.addQuad (edge_296, edge_333, edge_304, edge_202)
   face_334 = doc.addQuad (edge_336, edge_299, edge_205, edge_292)
   face_337 = doc.addQuad (edge_333, edge_306, edge_336, edge_294)
   face_34 = doc.addQuad (edge_20, edge_33, edge_28, edge_25)
   face_341 = doc.addQuad (edge_215, edge_326, edge_343, edge_318)
   face_344 = doc.addQuad (edge_343, edge_321, edge_333, edge_316)
   face_348 = doc.addQuad (edge_350, edge_352, edge_354, edge_105)
   face_355 = doc.addQuad (edge_98, edge_357, edge_359, edge_350)
   face_360 = doc.addQuad (edge_359, edge_362, edge_364, edge_352)
   face_365 = doc.addQuad (edge_354, edge_364, edge_367, edge_107)
   face_368 = doc.addQuad (edge_357, edge_83, edge_367, edge_362)
   face_372 = doc.addQuad (edge_245, edge_354, edge_374, edge_350)
   face_375 = doc.addQuad (edge_374, edge_364, edge_377, edge_359)
   face_378 = doc.addQuad (edge_377, edge_367, edge_235, edge_357)
   face_38 = doc.addQuad (edge_40, edge_43, edge_45, edge_47)
   face_48 = doc.addQuad (edge_40, edge_50, edge_52, edge_54)
   face_55 = doc.addQuad (edge_43, edge_54, edge_57, edge_59)
   face_60 = doc.addQuad (edge_62, edge_64, edge_59, edge_45)
   face_65 = doc.addQuad (edge_47, edge_62, edge_67, edge_50)
   face_68 = doc.addQuad (edge_64, edge_57, edge_52, edge_67)
   face_72 = doc.addQuad (edge_45, edge_74, edge_76, edge_78)
   face_79 = doc.addQuad (edge_74, edge_59, edge_81, edge_83)
   face_84 = doc.addQuad (edge_76, edge_86, edge_88, edge_83)
   face_89 = doc.addQuad (edge_78, edge_86, edge_91, edge_62)
   face_92 = doc.addQuad (edge_81, edge_64, edge_91, edge_88)
   face_96 = doc.addQuad (edge_76, edge_98, edge_100, edge_102)

   hexa_03 = doc.addHexa (face_04, face_14, face_21, face_26, face_31, face_34)
   hexa_119 = doc.addHexa (face_120, face_108, face_124, face_34, face_127, face_130)
   hexa_133 = doc.addHexa (face_134, face_144, face_151, face_156, face_161, face_164)
   hexa_167 = doc.addHexa (face_168, face_156, face_172, face_48, face_175, face_178)
   hexa_181 = doc.addHexa (face_182, face_186, face_26, face_189, face_14, face_192)
   hexa_195 = doc.addHexa (face_196, face_164, face_200, face_134, face_203, face_206)
   hexa_209 = doc.addHexa (face_210, face_178, face_213, face_168, face_200, face_216)
   hexa_219 = doc.addHexa (face_220, face_68, face_223, face_38, face_213, face_226)
   hexa_229 = doc.addHexa (face_230, face_92, face_233, face_72, face_223, face_236)
   hexa_239 = doc.addHexa (face_240, face_116, face_243, face_96, face_233, face_246)
   hexa_249 = doc.addHexa (face_250, face_130, face_192, face_120, face_243, face_252)
   hexa_255 = doc.addHexa (face_256, face_263, face_268, face_273, face_21, face_276)
   hexa_279 = doc.addHexa (face_280, face_189, face_273, face_283, face_263, face_286)
   hexa_289 = doc.addHexa (face_151, face_290, face_297, face_302, face_307, face_310)
   hexa_313 = doc.addHexa (face_314, face_172, face_307, face_319, face_324, face_327)
   hexa_330 = doc.addHexa (face_206, face_331, face_302, face_334, face_290, face_337)
   hexa_340 = doc.addHexa (face_216, face_341, face_327, face_331, face_314, face_344)
   hexa_347 = doc.addHexa (face_348, face_355, face_360, face_365, face_103, face_368)
   hexa_37 = doc.addHexa (face_38, face_48, face_55, face_60, face_65, face_68)
   hexa_371 = doc.addHexa (face_372, face_246, face_365, face_375, face_355, face_378)
   hexa_71 = doc.addHexa (face_72, face_60, face_79, face_84, face_89, face_92)
   hexa_95 = doc.addHexa (face_96, face_84, face_103, face_108, face_113, face_116)
   return doc

# ============================================================== Begin

if __name__ == '__main__' :
   doc = build ()
   mesh_hexas = mailler ()
