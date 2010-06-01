// Copyright (C) 2007-2010 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Martin Alnes, 2008
// Modified by Kent-Andre Mardal, 2009
// Modified by Ola Skavhaug, 2009
//
// First added:  2007-03-01
// Last changed: 2010-05-20

#ifndef __UFC_DOF_MAP_H
#define __UFC_DOF_MAP_H

#include <memory>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <dolfin/common/types.h>
#include <dolfin/common/Variable.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshFunction.h>
#include "GenericDofMap.h"
#include "UFC.h"
#include "UFCCell.h"
#include "UFCMesh.h"

namespace dolfin
{

  class UFC;
  template<class T> class Set;

  /// This class handles the mapping of degrees of freedom.
  /// It wraps a ufc::dof_map on a specific mesh and provides
  /// optional precomputation and reordering of dofs.

  class UFCDofMap : public GenericDofMap
  {
  public:

    /// Create dof map on mesh
    UFCDofMap(boost::shared_ptr<ufc::dof_map> ufc_dofmap,
              Mesh& dolfin_mesh);

    /// Create dof map on mesh (const mesh version)
    UFCDofMap(boost::shared_ptr<ufc::dof_map> ufc_dofmap,
              const Mesh& dolfin_mesh);

  public:

    /// Destructor
    ~UFCDofMap();

    /// Return a string identifying the dof map
    std::string signature() const
    {
      assert(_ufc_dofmap);
      return _ufc_dofmap->signature();
    }

    /// Return true iff mesh entities of topological dimension d are needed
    bool needs_mesh_entities(unsigned int d) const
    {
      assert(_ufc_dofmap);
      return _ufc_dofmap->needs_mesh_entities(d);
    }

    /// Return the dimension of the global finite element function space
    unsigned int global_dimension() const
    {
      assert(_ufc_dofmap);
      assert(_ufc_dofmap->global_dimension() > 0);
      return _ufc_dofmap->global_dimension();
    }

    /// Return the dimension of the local finite element function space on a cell
    unsigned int local_dimension(const ufc::cell& cell) const
    {
      assert(_ufc_dofmap);
      return _ufc_dofmap->local_dimension(cell);
    }

    /// Return the maximum dimension of the local finite element function space
    unsigned int max_local_dimension() const
    {
      assert(_ufc_dofmap);
      return _ufc_dofmap->max_local_dimension();
    }

    // Return the geometric dimension of the coordinates this dof map provides
    unsigned int geometric_dimension() const
    {
      assert(_ufc_dofmap);
      return _ufc_dofmap->geometric_dimension();
    }

    /// Return number of facet dofs
    unsigned int num_facet_dofs() const
    {
      assert(_ufc_dofmap);
      return _ufc_dofmap->num_facet_dofs();
    }

    /// Tabulate the local-to-global mapping of dofs on a cell (UFC cell version)
    void tabulate_dofs(uint* dofs, const ufc::cell& ufc_cell, uint cell_index) const;

    /// Tabulate the local-to-global mapping of dofs on a cell (DOLFIN cell version)
    void tabulate_dofs(uint* dofs, const Cell& cell) const;

    /// Tabulate local-local facet dofs
    void tabulate_facet_dofs(uint* dofs, uint local_facet) const;

    /// Tabulate the coordinates of all dofs on a cell (UFC cell version)
    void tabulate_coordinates(double** coordinates, const ufc::cell& ufc_cell) const
    { _ufc_dofmap->tabulate_coordinates(coordinates, ufc_cell); }

    /// Tabulate the coordinates of all dofs on a cell (DOLFIN cell version)
    void tabulate_coordinates(double** coordinates, const Cell& cell) const;

    /// Extract sub dofmap component
    UFCDofMap* extract_sub_dofmap(const std::vector<uint>& component, const Mesh& dolfin_mesh) const;

    /// "Collapse" a sub dofmap
    UFCDofMap* collapse(std::map<uint, uint>& collapsed_map, const Mesh& dolfin_mesh) const;

    /// Return the set of dof indices
    Set<dolfin::uint> dofs(const Mesh& mesh, bool sort = false) const;

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

  private:

    // Initialize
    void init(const Mesh& dolfin_mesh);

    /// Initialize the UFC mesh
    static void init_ufc_mesh(UFCMesh& ufc_mesh,
                              const Mesh& dolfin_mesh);

    /// Initialize the UFC dofmap
    static void init_ufc_dofmap(ufc::dof_map& dofmap,
                                const ufc::mesh ufc_mesh,
                                const Mesh& dolfin_mesh);

    // Recursively extract sub dofmap
    static ufc::dof_map* extract_sub_dofmap(const ufc::dof_map& ufc_dof_map,
                                            uint& offset,
                                            const std::vector<uint>& component,
                                            const ufc::mesh ufc_mesh,
                                            const Mesh& dolfin_mesh);

    // UFC dof map
    boost::shared_ptr<ufc::dof_map> _ufc_dofmap;

    // UFC mesh
    UFCMesh _ufc_mesh;

    // UFC dof map offset into parent's vector of coefficients
    uint _ufc_offset;

    // True iff running in parallel
    bool _parallel;

  };

}

#endif