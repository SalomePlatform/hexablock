#  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Makefile.am for Sphinx documentation of HEXABLOCK component
# Author: Marine Marcilhac From CS

.PHONY : latex

if SPHINX_IS_OK

hexablockdoc_DATA=html/index.html

html/index.html:$(RSTFILES)
	make htm

endif

dev_docs: build_dev_docs install_dev_docs

build_dev_docs:
	doxygen

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  html      to make standalone HTML files"
	@echo "  pickle    to make pickle files"
	@echo "  json      to make JSON files"
	@echo "  htmlhelp  to make HTML files and a HTML help project"
	@echo "  latex     to make LaTeX files, you can set PAPER=a4 or PAPER=letter"
	@echo "  changes   to make an overview over all changed/added/deprecated items"
	@echo "  linkcheck to check all external links for integrity"

DOXFILES=

EXTRA_DIST+= $(DOXFILES)

hexablockdocdir=$(docdir)/gui/HEXABLOCK
hexablockdevdocdir=$(docdir)/gui/HEXABLOCK/dev

SPHINXOPTS      =
SOURCEDIR       = $(srcdir)
SPHINXBUILD     = sphinx-build
PAPEROPT_a4     = -D latex_paper_size=a4
ALLSPHINXOPTS   = -d doctrees $(PAPEROPT_a4) $(SPHINXOPTS) $(SOURCEDIR)

htm:
	mkdir -p html doctrees
	$(SPHINXBUILD) -c $(top_builddir)/doc -b html $(ALLSPHINXOPTS) html
	@echo
	@echo "Build finished. The HTML pages are in html."

latex:
	mkdir -p latex doctrees
	$(SPHINXBUILD) -c $(top_builddir)/doc -b latex $(ALLSPHINXOPTS) latex
	@echo
	@echo "Build finished; the LaTeX files are in latex."
	@echo "Run \`make all-pdf' or \`make all-ps' in that directory to" \
	      "run these through (pdf)latex."

pdf:
	$(SPHINXBUILD) -c $(top_builddir)/doc -b pdf $(ALLSPHINXOPTS) pdf
	@echo
	@echo "Build finished. The pdf documents are in pdf."

html:
		 mkdir -p $@

htmldev:
		 mkdir -p $@

RSTFILES= \
	annexe.rst                  \
	block_method.rst            \
	cad.rst                     \
	creategrids.rst             \
	full_example_bridle.rst     \
	general.rst                 \
	gui.rst                     \
	gui_asso_quad_to_geom.rst   \
	gui_blocks_for_cyl_pipe.rst \
	gui_component.rst           \
	gui_cut_hexa.rst            \
	gui_cyl.rst                 \
	gui_disc_elmts.rst          \
	gui_discret_law.rst         \
	gui_document.rst            \
	gui_edge.rst                \
	gui_elements.rst            \
	gui_groups.rst              \
	gui_hexahedron.rst          \
	gui_make_elmts.rst          \
	gui_make_symmetry.rst       \
	gui_merge_elmts.rst         \
	gui_mesh.rst                \
	gui_modify_elmts.rst        \
	gui_modify_symmetry.rst     \
	gui_pipe.rst                \
	gui_prism_join_quad.rst     \
	gui_propag.rst              \
	gui_quadrangle.rst          \
	gui_remove.rst              \
	gui_vector.rst              \
	gui_vertex.rst              \
	index.rst                   \
	interactive.rst             \
	intro_association.rst       \
	intro_groups.rst            \
	intro_mesh.rst              \
	intro_python.rst            \
	mesh.rst                    \
	python.rst                  \
	tui.rst                     \
	tui_asso_quad_to_geom.rst   \
	tui_blocks_for_cyl_pipe.rst \
	tui_cartgrid.rst            \
	tui_component.rst           \
	tui_cut_hexa.rst            \
	tui_cylgrid.rst             \
	tui_cyl.rst                 \
	tui_disc_elmts.rst          \
	tui_discret_law.rst         \
	tui_document.rst            \
	tui_edge.rst                \
	tui_elements.rst            \
	tui_groups.rst              \
	tui_hexahedron.rst          \
	tui_make_elmts.rst          \
	tui_merge_elmts.rst         \
	tui_mesh.rst                \
	tui_modify_elmts.rst        \
	tui_pipe.rst                \
	tui_prism_join_quad.rst     \
	tui_propag.rst              \
	tui_quadrangle.rst          \
	tui_remove.rst              \
	tui_sphergrid.rst           \
	tui_vector.rst              \
	tui_vertex.rst



EXTRA_DIST+= $(RSTFILES) _static cas_test_python pyplots

install-data-local:
	$(INSTALL) -d $(DESTDIR)$(hexablockdocdir)
	if test -d "html"; then b=; else b="$(srcdir)/"; fi; \
	cp -rf $$b"html"/* $(DESTDIR)$(hexablockdocdir) ; \
	if test -f $$b"latex"/using.pdf; then cp -f $$b"latex"/using.pdf $(DESTDIR)$(hexablockdocdir) ; fi; \
	if test -f $$b"latex"/integration.pdf; then cp -f $$b"latex"/integration.pdf $(DESTDIR)$(hexablockdocdir) ; fi;

install_dev_docs:
	$(INSTALL) -d $(DESTDIR)$(hexablockdevdocdir)
	if test -d "htmldev"; then b=; else b="$(srcdir)/"; fi; \
	find $$b"htmldev" -name "*.*" -exec cp -rf {} $(DESTDIR)$(hexablockdevdocdir) ';' ; \
	cp -rf $(top_srcdir)/src/hexablockloader/samples $(DESTDIR)$(hexablockdevdocdir); \
	rm -rf $(DESTDIR)$(hexablockdevdocdir)/samples/CVS;

uninstall-local:
	chmod -R +w $(DESTDIR)$(hexablockdocdir)
	rm -rf $(DESTDIR)$(hexablockdocdir)/*

clean-local:
	-rm -rf html latex doctrees
	if test -d "html"; then rm -rf html ; fi

dist-hook:
	if test -d "html"; then b=; else b="$(srcdir)/"; fi; \
	if test -d $$b"html"; then cp -rf $$b"html" $(distdir) ; fi; \
	if test -d "htmldev"; then b=; else b="$(srcdir)/"; fi; \
	if test -d $$b"htmldev"; then cp -rf $$b"htmldev" $(distdir) ; fi
