// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_MANAGER_DRIVER_H_
#define COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_MANAGER_DRIVER_H_

#include <map>
#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/string16.h"
#include "components/autofill/core/common/password_form_field_prediction_map.h"

namespace autofill {
class AutofillManager;
struct FormData;
struct FormFieldData;
struct PasswordForm;
struct PasswordFormFillData;
}  // namespace autofill

namespace password_manager {

class PasswordAutofillManager;
class PasswordGenerationManager;
class PasswordManager;

// Interface that allows PasswordManager core code to interact with its driver
// (i.e., obtain information from it and give information to it).
class PasswordManagerDriver
    : public base::SupportsWeakPtr<PasswordManagerDriver> {
 public:
  PasswordManagerDriver() {}
  virtual ~PasswordManagerDriver() {}

  // Fills forms matching |form_data|.
  virtual void FillPasswordForm(
      const autofill::PasswordFormFillData& form_data) = 0;

  // Informs the driver that |form| can be used for password generation.
  virtual void AllowPasswordGenerationForForm(
      const autofill::PasswordForm& form) = 0;

  // Notifies the driver that account creation |forms| were found.
  virtual void AccountCreationFormsFound(
      const std::vector<autofill::FormData>& forms) = 0;

  // Notifies the driver that username and password predictions from autofill
  // have been received.
  virtual void AutofillDataReceived(
      const std::map<autofill::FormData,
                     autofill::PasswordFormFieldPredictionMap>& predictions) {}

  // Notifies the driver that the user has accepted a generated password.
  virtual void GeneratedPasswordAccepted(const base::string16& password) = 0;

  // Tells the driver to fill the form with the |username| and |password|.
  virtual void FillSuggestion(const base::string16& username,
                              const base::string16& password) = 0;

  // Tells the driver to preview filling form with the |username| and
  // |password|.
  virtual void PreviewSuggestion(const base::string16& username,
                                 const base::string16& password) = 0;

  // Tells the driver to clear previewed password and username fields.
  virtual void ClearPreviewedForm() = 0;

  // Tells the driver to find the focused password field and report back
  // the corresponding password form, so that it can be saved.
  virtual void ForceSavePassword() {}

  // Returns the PasswordGenerationManager associated with this instance.
  virtual PasswordGenerationManager* GetPasswordGenerationManager() = 0;

  // Returns the PasswordManager associated with this instance.
  virtual PasswordManager* GetPasswordManager() = 0;

  // Returns the PasswordAutofillManager associated with this instance.
  virtual PasswordAutofillManager* GetPasswordAutofillManager() = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(PasswordManagerDriver);
};

}  // namespace password_manager

#endif  // COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_PASSWORD_MANAGER_DRIVER_H_