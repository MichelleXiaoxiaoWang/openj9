/*******************************************************************************
* Copyright (c) 2017, 2018 IBM Corp. and others
*
* This program and the accompanying materials are made available under
* the terms of the Eclipse Public License 2.0 which accompanies this
* distribution and is available at https://www.eclipse.org/legal/epl-2.0/
* or the Apache License, Version 2.0 which accompanies this distribution and
* is available at https://www.apache.org/licenses/LICENSE-2.0.
*
* This Source Code may also be made available under the following
* Secondary Licenses when the conditions for such availability set
* forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
* General Public License, version 2 with the GNU Classpath
* Exception [1] and GNU General Public License, version 2 with the
* OpenJDK Assembly Exception [2].
*
* [1] https://www.gnu.org/software/classpath/license.html
* [2] http://openjdk.java.net/legal/assembly-exception.html
*
* SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
*******************************************************************************/

#include "optimizer/RecognizedCallTransformer.hpp"

#include "compile/ResolvedMethod.hpp"
#include "env/CompilerEnv.hpp"
#include "env/VMJ9.h"
#include "env/jittypes.h"
#include "il/Block.hpp"
#include "il/Node.hpp"
#include "il/Node_inlines.hpp"
#include "il/TreeTop.hpp"
#include "il/TreeTop_inlines.hpp"
#include "il/symbol/StaticSymbol.hpp"
#include "il/ILOpCodes.hpp"
#include "il/ILOps.hpp"                                   // for ILOpCode, etc
#include "ilgen/IlGenRequest.hpp"
#include "ilgen/IlGeneratorMethodDetails.hpp"
#include "ilgen/IlGeneratorMethodDetails_inlines.hpp"
#include "optimizer/CallInfo.hpp"
#include "optimizer/Structure.hpp"
#include "codegen/CodeGenerator.hpp"                      // for CodeGenerator
#include "optimizer/TransformUtil.hpp"

void J9::RecognizedCallTransformer::processIntrinsicFunction(TR::TreeTop* treetop, TR::Node* node, TR::ILOpCodes opcode)
   {
   TR::Node::recreate(node, opcode);
   TR::TransformUtil::removeTree(comp(), treetop);
   }

bool J9::RecognizedCallTransformer::isInlineable(TR::TreeTop* treetop)
   {
   auto node = treetop->getNode()->getFirstChild();
   switch(node->getSymbol()->castToMethodSymbol()->getMandatoryRecognizedMethod())
      {
      case TR::java_lang_Integer_rotateLeft:
      case TR::java_lang_Long_rotateLeft:
      case TR::java_lang_Math_abs_I:
      case TR::java_lang_Math_abs_L:
      case TR::java_lang_Math_abs_F:
      case TR::java_lang_Math_abs_D:
         return TR::Compiler->target.cpu.isX86() ||
            TR::Compiler->target.cpu.isZ();
      case TR::java_lang_Math_max_I:
      case TR::java_lang_Math_min_I:
      case TR::java_lang_Math_max_L:
      case TR::java_lang_Math_min_L:
         return !comp()->getOption(TR_DisableMaxMinOptimization);
      default:
         return false;
      }
   }

void J9::RecognizedCallTransformer::transform(TR::TreeTop* treetop)
   {
   auto node = treetop->getNode()->getFirstChild();
   switch(node->getSymbol()->castToMethodSymbol()->getMandatoryRecognizedMethod())
      {
      case TR::java_lang_Integer_rotateLeft:
         processIntrinsicFunction(treetop, node, TR::irol);
         break;
      case TR::java_lang_Long_rotateLeft:
         processIntrinsicFunction(treetop, node, TR::lrol);
         break;
      case TR::java_lang_Math_abs_I:
         processIntrinsicFunction(treetop, node, TR::iabs);
         break;
      case TR::java_lang_Math_abs_L:
         processIntrinsicFunction(treetop, node, TR::labs);
         break;
      case TR::java_lang_Math_abs_D:
         processIntrinsicFunction(treetop, node, TR::dabs);
         break;
      case TR::java_lang_Math_abs_F:
         processIntrinsicFunction(treetop, node, TR::fabs);
         break;
      case TR::java_lang_Math_max_I:
         processIntrinsicFunction(treetop, node, TR::imax);
         break;
      case TR::java_lang_Math_min_I:
         processIntrinsicFunction(treetop, node, TR::imin);
         break;
      case TR::java_lang_Math_max_L:
         processIntrinsicFunction(treetop, node, TR::lmax);
         break;
      case TR::java_lang_Math_min_L:
         processIntrinsicFunction(treetop, node, TR::lmin);
         break;
      default:
         break;
      }
   }
